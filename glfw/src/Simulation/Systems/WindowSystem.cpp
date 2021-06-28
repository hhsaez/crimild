/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "WindowSystem.hpp"

#include "Concurrency/Async.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/Systems/RenderSystem.hpp"

using namespace crimild;
using namespace crimild::glfw;

#if defined( CRIMILD_ENABLE_VULKAN )

    #include "Rendering/VulkanInstance.hpp"
    #include "Rendering/VulkanSurface.hpp"

using namespace crimild::vulkan;

#endif

void WindowSystem::onInit( void ) noexcept
{
    if ( !createWindow() ) {
        return;
    }
    /*
    Log::info( CRIMILD_CURRENT_CLASS_NAME, "Initialized GLFW ", versionMajor, ".", versionMinor, " rev. ", versionRevision );


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

#if 0
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
#endif

    crimild::concurrency::sync_frame( std::bind( &WindowSystem::update, this ) );

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
	emscripten_set_main_loop( simulation_step, 60, false );
#endif
	*/
}

void WindowSystem::update( void ) noexcept
{
    /*
    CRIMILD_PROFILE( "Window System - Update" )

	static double accumTime = 0.0;
	double currentTime = glfwGetTime();
	double delta = currentTime - accumTime;
	accumTime = currentTime;

	glfwPollEvents();

	if ( glfwWindowShouldClose( m_window ) ) {
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
	*/

    if ( Simulation::getInstance()->getSettings()->get( "video.show_frame_time", true ) ) {
        auto sim = Simulation::getInstance();
        auto name = sim->getName();
        auto clock = sim->getSimulationClock();
        std::stringstream ss;
        ss << name << " (" << clock.getDeltaTime() << "ms)";
        glfwSetWindowTitle( m_window, ss.str().c_str() );
    }
}

void WindowSystem::onTerminate( void ) noexcept
{
    destroyWindow();
}

bool WindowSystem::createWindow( void )
{
    auto settings = Simulation::getInstance()->getSettings();

    auto width = settings->get< crimild::Int32 >( "video.width", 1024 );
    auto height = settings->get< crimild::Int32 >( "video.height", 768 );
    auto fullscreen = settings->get< crimild::Bool >( "video.fullscreen", false );

    // disable Retina by default
    const auto enableHDPI = settings->get< crimild::Bool >( "video.hdpi", false );

	// use discrete GPU by default, by disabling automatic graphics switching
    const auto enableGraphicsSwitching = settings->get< crimild::Bool >( "video.graphicsSwitching", false );

    auto simName = Simulation::getInstance()->getName();
    if ( simName == "" ) {
        simName = "Crimild";
    }
    auto vsync = settings->get< crimild::Bool >( "video.vsync", true );

    CRIMILD_LOG_DEBUG( "Creating window of size (", width, "x", height, ")" );

#if defined( CRIMILD_PLATFORM_EMSCRIPTEN )
    glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
#elif defined( CRIMILD_ENABLE_VULKAN )
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
#else
    int glMajor = 3;
    int glMinor = 3;
    int depthBits = 24;
    int stencilBits = 8;
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, glMajor );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, glMinor );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint( GLFW_DEPTH_BITS, depthBits );
    glfwWindowHint( GLFW_STENCIL_BITS, stencilBits );
#endif

    glfwWindowHint( GLFW_COCOA_RETINA_FRAMEBUFFER, enableHDPI ? GLFW_TRUE : GLFW_FALSE );
    glfwWindowHint( GLFW_COCOA_GRAPHICS_SWITCHING, enableGraphicsSwitching ? GLFW_TRUE : GLFW_FALSE );

    m_window = glfwCreateWindow(
        width,
        height,
        simName.c_str(),
        fullscreen ? glfwGetPrimaryMonitor() : NULL,
        NULL );
    if ( m_window == nullptr ) {
        CRIMILD_LOG_FATAL( "Failed to create window" );
        exit( 1 );
    }

    glfwSetWindowSizeCallback(
        m_window,
        []( GLFWwindow *window, int width, int height ) {
            Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Window resized to ", width, "x", height );
        } );

    glfwSetFramebufferSizeCallback(
        m_window,
        []( GLFWwindow *window, int width, int height ) {
            Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Framebuffer resized to ", width, "x", height );
        } );

    broadcastMessage( messages::WindowSystemDidCreateWindow { this } );

#if !defined( CRIMILD_ENABLE_VULKAN )
    glfwMakeContextCurrent( _window );
    glfwSwapInterval( vsync ? 1 : 0 );
#endif

	Real framebufferScale = 1.0f;
    Int32 framebufferWidth = 0;
    Int32 framebufferHeight = 0;
	glfwGetFramebufferSize( m_window, &framebufferWidth, &framebufferHeight);
	if ( enableHDPI && framebufferWidth > 0 ) {
		framebufferScale = Real( framebufferWidth ) / Real( width );
    }


    // Make sure we have proper windows settings defined
    settings->set( "video.width", width );
    settings->set( "video.height", height );
    settings->set( "video.fullscreen", fullscreen );
    settings->set( "video.framebufferScale", framebufferScale );

    return true;
}

void WindowSystem::destroyWindow( void )
{
    broadcastMessage( messages::WindowSystemWillDestroyWindow { this } );

    // TODO: window is automatically destroyed when terminating simulator
    // Calling this function here may result in an error if the window was
    // already closed by the user (i.e. by pressing the "X" button) instead
    // of triggering a termination by other means (i.e. pressing ESC).
    //glfwDestroyWindow( m_window );
}
