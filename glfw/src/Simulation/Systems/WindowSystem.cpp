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
    Log::info( CRIMILD_CURRENT_CLASS_NAME, "Initialized GLFW ", versionMajor, ".", versionMinor, " rev. ", versionRevision );

	if ( !createWindow() ) {
		return false;
	}

    int framebufferWidth = 0;
    int framebufferHeight = 0;
	glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight);

    // this is not working
//    bool supersampling = Simulation::getInstance()->getSettings()->get( "video.supersampling", true );
//#ifdef CRIMILD_PLATFORM_WIN32
//	supersampling = false;
//#endif
//    if ( supersampling ) {
//	    int windowWidth = Simulation::getInstance()->getSettings()->get( "video.width", 1024 );
//    	int windowHeight = Simulation::getInstance()->getSettings()->get( "video.height", 768 );
//
//    	framebufferWidth = Numerici::max( framebufferWidth, 2 * windowWidth );
//    	framebufferHeight = Numerici::max( framebufferHeight, 2 * windowHeight );
//    }
//    else {
//	    framebufferWidth = Simulation::getInstance()->getSettings()->get( "video.width", 1024 );
//    	framebufferHeight = Simulation::getInstance()->getSettings()->get( "video.height", 768 );
//    }

    auto renderer = Simulation::getInstance()->getRenderer();
    auto screenBuffer = renderer->getScreenBuffer();
 	screenBuffer->resize( framebufferWidth, framebufferHeight );
    renderer->configure();
    
    crimild::concurrency::sync_frame( std::bind( &WindowSystem::update, this ) );

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
    
	if ( glfwWindowShouldClose( _window ) ) {
        crimild::concurrency::sync_frame( [] {
            Simulation::getInstance()->stop();
        });
		return;
	}

    broadcastMessage( crimild::messaging::RenderNextFrame {} );
	broadcastMessage( crimild::messaging::PresentNextFrame {} );
	glfwSwapBuffers( _window );

    if ( delta < 0.002 ) {
        // this trick prevents the simulation to run at very high speeds
        // this usually happens when the window is sent to background
        // and there is nothing to render
        int sleepTime = ( int )( ( 1.0 / 60.0 - delta ) * 1000.0 );
        std::this_thread::sleep_for( std::chrono::milliseconds( sleepTime ) );
    }

	std::string name = Simulation::getInstance()->getName();
	std::stringstream ss;
	ss << name << " (" << delta << "ms)";
	glfwSetWindowTitle( _window, ss.str().c_str() );
    
    crimild::concurrency::sync_frame( std::bind( &WindowSystem::update, this ) );
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

	std::string name = Simulation::getInstance()->getName();
	if ( name == "" ) {
		name = "Crimild";
	}
	
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
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Failed to create window" );
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

