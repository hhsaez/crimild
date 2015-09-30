#include "WindowSystem.hpp"

using namespace crimild;

WindowSystem::WindowSystem( void )
	: System( "Window System" )
{

}

WindowSystem::~WindowSystem( void )
{

}

bool WindowSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}

	int versionMajor;
	int versionMinor;
	int versionRevision;
	glfwGetVersion( &versionMajor, &versionMinor, &versionRevision );
	Log::Info << "Initialized GLFW " << versionMajor << "." << versionMinor << " rev. " << versionRevision << Log::End;

	if ( !createWindow() ) {
		return false;
	}

    int framebufferWidth;
    int framebufferHeight;
	glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight);

    auto screenBuffer = crimild::alloc< FrameBufferObject >( framebufferWidth, framebufferHeight );
    auto renderer = Simulation::getInstance()->getRenderer();
    renderer->setScreenBuffer( screenBuffer );
    renderer->configure();
    
    crimild::async( crimild::AsyncDispatchPolicy::MAIN_QUEUE, std::bind( &WindowSystem::update, this ) );

	return true;
}

void WindowSystem::update( void )
{
    CRIMILD_PROFILE( "Window System - Update" )
    
	static double accumTime = 0.0;
	double currentTime = glfwGetTime();
	double delta = currentTime - accumTime;
	accumTime = currentTime;

	glfwPollEvents();
    
	if ( !glfwWindowShouldClose( _window ) ) {
		glfwSwapBuffers( _window );

	    if ( delta < 0.002 ) {
	        // this trick prevents the simulation to run at very high speeds
	        // this usually happens when the window is sent to background
	        // and there is nothing to render
	        int sleepTime = ( int )( ( 1.0 / 60.0 - delta ) * 1000.0 );
	        std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );
	    }
	}
	else {
		Simulation::getInstance()->stop();
	}

    crimild::async( crimild::AsyncDispatchPolicy::MAIN_QUEUE, std::bind( &WindowSystem::update, this ) );
}

void WindowSystem::stop( void )
{
	System::stop();
}

bool WindowSystem::createWindow( void )
{
    int width = Simulation::getInstance()->getSettings()->get( "video.width", 1024 );
    int height = Simulation::getInstance()->getSettings()->get( "video.height", 768 );
    bool fullscreen = Simulation::getInstance()->getSettings()->get< bool >( "video.fullscreen", false );

	std::string name = "Crimild";
	bool vsync = true;
	int glMajor = 3;
	int glMinor = 2;
	int depthBits = 32;
	int stencilBits = 8;

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, glMajor );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, glMinor );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint( GLFW_DEPTH_BITS, depthBits );
    glfwWindowHint( GLFW_STENCIL_BITS, stencilBits );

	_window = glfwCreateWindow( width, height, name.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL );
	if ( _window == nullptr ) {
		Log::Error << "Failed to create window" << Log::End;
		return false;
	}

  	glfwMakeContextCurrent( _window );

	glfwSwapInterval( vsync ? 1 : 0 );

	broadcastMessage( messages::WindowSystemDidCreateWindow { this } );

	return true;
}

void WindowSystem::destroyWindow( void )
{
	broadcastMessage( messages::WindowSystemWillDestroyWindow { this } );
}

