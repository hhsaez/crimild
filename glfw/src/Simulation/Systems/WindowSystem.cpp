#include "WindowSystem.hpp"

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
    #include <emscripten.h>
    #include <emscripten/html5.h>

void simulation_step( void )
{
	crimild::Simulation::getInstance()->update();
}

#endif

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

	int framebufferWidth = Simulation::getInstance()->getSettings()->get( "video.width", 1024 );
    int framebufferHeight = Simulation::getInstance()->getSettings()->get( "video.height", 768 );

#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
    framebufferWidth = 0;
    framebufferHeight = 0;
	glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight);
#endif

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

	Log::info( CRIMILD_CURRENT_CLASS_NAME, "Framebuffer size (", framebufferWidth, "x", framebufferHeight, ")" );

    auto renderer = Simulation::getInstance()->getRenderer();
    auto screenBuffer = renderer->getScreenBuffer();
 	screenBuffer->resize( framebufferWidth, framebufferHeight );
    renderer->configure();

	// compute new aspect ratio for main camera, if any
	if ( Simulation::getInstance()->getMainCamera() != nullptr ) {
		auto aspect = ( float ) screenBuffer->getWidth() / ( float ) screenBuffer->getHeight();
		Simulation::getInstance()->getMainCamera()->setAspectRatio( aspect );
	}

	/*
#ifdef __EMSCRIPTEN__
    int result = emscripten_webgl_enable_extension(emscripten_webgl_get_current_context(), "OES_texture_float");
    if (result < 0) {
        std::cerr << "Could not load extension \"OES_texture_float\"\n";
        emscripten_run_script("alert('Could not load extension \"OES_texture_float\". Game will not work.')");
    }
    result = emscripten_webgl_enable_extension(emscripten_webgl_get_current_context(), "OES_texture_float_linear");
    if (result < 0) {
        std::cerr << "Could not load extension \"OES_texture_float_linear\"\n";
        emscripten_run_script("alert('Could not load extension \"OES_texture_float_linear\". Game will not work.')");
    }
#endif
	*/
    
    crimild::concurrency::sync_frame( std::bind( &WindowSystem::update, this ) );

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN 
	emscripten_set_main_loop( simulation_step, 60, false );
#endif

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

	if ( Simulation::getInstance()->getSettings()->get( "video.show_frame_time", false ) ) {
		std::string name = Simulation::getInstance()->getName();
		std::stringstream ss;
		ss << name << " (" << delta << "ms)";
		glfwSetWindowTitle( _window, ss.str().c_str() );
	}

    crimild::concurrency::sync_frame( std::bind( &WindowSystem::update, this ) );
}

void WindowSystem::stop( void )
{
	System::stop();
}

bool WindowSystem::createWindow( void )
{
	int width = 1024;
	int height = 768;
	bool fullscreen = false;

#if defined( CRIMILD_PLATFORM_EMSCRIPTEN )
	// override window size based on canvas
	if ( emscripten_get_canvas_element_size( "crimild_canvas", &width, &height ) != EMSCRIPTEN_RESULT_SUCCESS ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot obtain canvas size" );
	}

	Log::info( CRIMILD_CURRENT_CLASS_NAME, "Canvas size = (", width, "x", height, ")" );

	Simulation::getInstance()->getSettings()->set( "video.width", width );
	Simulation::getInstance()->getSettings()->set( "video.height", height );
	Simulation::getInstance()->getSettings()->set( "video.fullscreen", fullscreen );	
#endif
	
    width = Simulation::getInstance()->getSettings()->get( "video.width", 1024 );
    height = Simulation::getInstance()->getSettings()->get( "video.height", 768 );
    fullscreen = Simulation::getInstance()->getSettings()->get< bool >( "video.fullscreen", false );

	Log::info( CRIMILD_CURRENT_CLASS_NAME, "Creating window of size (", width, "x", height, ")" );

	std::string name = Simulation::getInstance()->getName();
	if ( name == "" ) {
		name = "Crimild";
	}
	
	bool vsync = true;
	int glMajor = 3;
	int glMinor = 3;
	int depthBits = 32;
	int stencilBits = 8;

#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, glMajor );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, glMinor );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint( GLFW_DEPTH_BITS, depthBits );
    glfwWindowHint( GLFW_STENCIL_BITS, stencilBits );
#else
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#endif

	_window = glfwCreateWindow( width, height, name.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL );
	if ( _window == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Failed to create window" );
		return false;
	}

  	glfwMakeContextCurrent( _window );

	glfwSwapInterval( vsync ? 1 : 0 );

	broadcastMessage( messages::WindowSystemDidCreateWindow { this } );

	glfwSetWindowSizeCallback( _window, []( GLFWwindow *window, int width, int height ) {
		Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Window resized to ", width, "x", height );
	});

	glfwSetFramebufferSizeCallback( _window, []( GLFWwindow *window, int width, int height ) {
		Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Framebuffer resized to ", width, "x", height );
	});

	return true;
}

void WindowSystem::destroyWindow( void )
{
	broadcastMessage( messages::WindowSystemWillDestroyWindow { this } );
}

