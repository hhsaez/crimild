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

#include "Rendering/GLFWWindow.hpp"

#include "Foundation/VulkanUtils.hpp"
#include "Rendering/RenderPasses/VulkanClearPass.hpp"
#include "Rendering/RenderPasses/VulkanPresentPass.hpp"
#include "Rendering/RenderPasses/VulkanScenePass.hpp"
#include "Rendering/RenderPasses/VulkanShaderPass.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Simulation/Settings.hpp"

#include <array>
#include <iomanip>

using namespace crimild;
using namespace crimild::glfw;

class ComposePass : public vulkan::RenderPass {
public:
    ComposePass( vulkan::RenderDevice *renderDevice )
        : m_clear( renderDevice ),
          m_scene( renderDevice ),
          m_shader(
              renderDevice,
              R"(
                float circleMask( vec2 uv, vec2 p, float r, float blur )
                {
                    float d = length( uv - p );
                    float c = smoothstep( r, r - blur, d );
                    return c;
                }

                void main()
                {
                    vec2 uv = inTexCoord;
                    uv -= 0.5;
                    uv.x *= context.dimensions.x / context.dimensions.y;

                    float blur = 0.00625;

                    float mask = circleMask( uv, vec2( 0.0 ), 0.4, blur );
                    mask -= circleMask( uv, vec2( -0.15, 0.1 ), 0.075, blur );
                    mask -= circleMask( uv, vec2( 0.15, 0.1 ), 0.075, blur );
                    vec3 faceColor = vec3( 1.0, 1.0, 0.0 ) * mask;

                    mask = circleMask( uv, vec2( 0.0 ), 0.25, blur );
                    mask -= circleMask( uv, vec2( 0.0, 0.05 ), 0.25, blur );
                    mask *= uv.y <= 0.0 ? 1.0 : 0.0;
                    vec3 mouthColor = vec3( 1.0 ) * mask;

                    vec3 color = faceColor - mouthColor;

                    outColor = vec4( color, 1.0 );
                }
            )" ),
          m_present( renderDevice )
    {
    }

    virtual ~ComposePass( void ) = default;

    virtual void render( void ) noexcept override
    {
        m_clear.render();
        // m_scene.render();
        m_shader.render();
        m_present.render();
    }

    virtual void handle( const Event &e ) noexcept override
    {
        m_clear.handle( e );
        // m_scene.handle( e );
        m_shader.handle( e );
        m_present.handle( e );
    }

private:
    vulkan::ClearPass m_clear;
    vulkan::ScenePass m_scene;
    vulkan::ShaderPass m_shader;
    vulkan::PresentPass m_present;
};

Window::Window( void ) noexcept
{
    if ( !createWindow() ) {
        exit( -1 );
    }

    // The instance should be independent from the window. What if we have multiple windows?
    m_instance = std::make_unique< vulkan::VulkanInstance >();

    // TODO: I think the surface should be created by the instance
    m_surface = std::make_unique< glfw::VulkanSurface >( m_instance.get(), this );

    // The physical device should be independent from the window. What if we have multiple windows?
    m_physicalDevice = m_instance->createPhysicalDevice( m_surface.get() );

    m_renderDevice = m_physicalDevice->createRenderDevice();

    m_renderPass = std::make_unique< ComposePass >( m_renderDevice.get() );
}

Window::~Window( void ) noexcept
{
    if ( m_renderDevice != nullptr ) {
        m_renderDevice->flush();
    }

    m_renderPass = nullptr;
    m_renderDevice = nullptr;
    m_physicalDevice = nullptr;
    m_surface = nullptr;
    m_instance = nullptr;

    destroyWindow();
}

Event Window::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::TICK: {
            if ( glfwWindowShouldClose( m_window ) ) {
                return Event { .type = Event::Type::TERMINATE };
            }

            if ( m_lastResizeEvent.type != Event::Type::NONE ) {
                m_renderDevice->handle( m_lastResizeEvent );
                m_renderPass->handle( m_lastResizeEvent );
                m_lastResizeEvent = Event {};
            }

            if ( m_renderDevice->beginRender() ) {
                if ( m_renderPass != nullptr ) {
                    m_renderPass->render();
                }
                m_renderDevice->endRender();
            }

            // if ( Settings->getInstance()->get( "video.show_frame_time", true ) ) {
            //     auto name = sim->getName();
            //     auto clock = sim->getSimulationClock();
            //     std::stringstream ss;
            //     auto accum = clock.getAccumTime();
            //     auto h = Int32( accum / 3600 );
            //     auto m = Int32( ( accum - h * 3600 ) / 60 );
            //     auto s = Int32( ( accum - h * 3600 - m * 60 ) );
            //     ss << name << " ("
            //        << std::fixed
            //        << std::setprecision( 3 )
            //        << clock.getDeltaTime() << "ms - "
            //        << ( h < 10 ? "0" : "" ) << h << ":"
            //        << ( m < 10 ? "0" : "" ) << m << ":"
            //        << ( s < 10 ? "0" : "" ) << s << ")";
            //     glfwSetWindowTitle( m_window, ss.str().c_str() );
            // }

            break;
        }

        case Event::Type::WINDOW_RESIZE: {
            CRIMILD_LOG_DEBUG( "Window resized to ", e.extent.width, "x", e.extent.height );
            // Update settings before forwarding events
            auto settings = Settings::getInstance();
            settings->set( "video.width", e.extent.width );
            settings->set( "video.height", e.extent.height );

            // Delay event until next render, so we only forward it once after the resizing is completed
            m_lastResizeEvent = e;
            break;
        }

        default: {
            break;
        }
    }

    return e; //m_surface->handle( e );

    // if ( e.type == Event::Type::SIMULATION_START ) {
    // }

    // if ( e.type == Event::Type::SIMULATION_UPDATE ) {

    // }

    // const auto ret = System::handle( e );

    // if ( ret.type == Event::Type::SIMULATION_STOP ) {
    //     destroyWindow();
    // }

    // return ret;
    // return e; // System::handle( e );
}

#if 0
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
        auto accum = clock.getAccumTime();
        auto h = Int32( accum / 3600 );
        auto m = Int32( ( accum - h * 3600 ) / 60 );
        auto s = Int32( ( accum - h * 3600 - m * 60 ) );
        ss << name << " ("
           << std::fixed
           << std::setprecision( 3 )
           << clock.getDeltaTime() << "ms - "
           << ( h < 10 ? "0" : "" ) << h << ":"
           << ( m < 10 ? "0" : "" ) << m << ":"
           << ( s < 10 ? "0" : "" ) << s << ")";
        glfwSetWindowTitle( m_window, ss.str().c_str() );
    }
}

void WindowSystem::onTerminate( void ) noexcept
{
    destroyWindow();
}
#endif

bool Window::createWindow( void )
{
    auto settings = Settings::getInstance();

    auto width = settings->get< crimild::Int32 >( "video.width", 1024 );
    auto height = settings->get< crimild::Int32 >( "video.height", 768 );
    auto fullscreen = settings->get< crimild::Bool >( "video.fullscreen", false );

    if ( settings->hasKey( "video.resolution" ) ) {
        auto resolution = settings->get< std::string >( "video.resolution", "hd" );
        if ( resolution == "480p" ) {
            width = 640;
            height = 480;
        } else if ( resolution == "sd" ) {
            width = 720;
            height = 576;
        } else if ( resolution == "hd" || resolution == "720p" ) {
            width = 1280;
            height = 720;
        } else if ( resolution == "fullHD" || resolution == "2k" || resolution == "1080p" ) {
            width = 1920;
            height = 1080;
        } else if ( resolution == "uhd" ) {
            width = 3840;
            height = 2160;
        } else if ( resolution == "4k" ) {
            width = 4096;
            height = 2160;
        } else if ( resolution == "8k" ) {
            width = 7680;
            height = 4230;
        }
    }

    // disable Retina by default
    const auto enableHDPI = settings->get< crimild::Bool >( "video.hdpi", false );

    // use discrete GPU by default, by disabling automatic graphics switching
    const auto enableGraphicsSwitching = settings->get< crimild::Bool >( "video.graphicsSwitching", false );

    auto simName = settings->get< std::string >( Settings::SETTINGS_APP_NAME, "Crimild" );

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

    // Set the user pointer to this object so it can be used for different events
    glfwSetWindowUserPointer( m_window, this );

    glfwSetWindowSizeCallback(
        m_window,
        []( GLFWwindow *windowHandle, int width, int height ) {
            auto window = static_cast< Window * >( glfwGetWindowUserPointer( windowHandle ) );
            if ( window != nullptr ) {
                window->handle(
                    Event {
                        .type = Event::Type::WINDOW_RESIZE,
                        .extent = {
                            .width = Real32( width ),
                            .height = Real32( height ),
                        } } );
            } else {
                CRIMILD_LOG_WARNING( "glfwSetWindowSizeCallback: GLFW window user pointer not set" );
            }
        } );

#if !defined( CRIMILD_ENABLE_VULKAN )
    glfwMakeContextCurrent( _window );
    auto vsync = settings->get< crimild::Bool >( "video.vsync", true );
    glfwSwapInterval( vsync ? 1 : 0 );
#endif

    // Compute framebuffer scale
    // This is doen only once. Resizing the window does not require to recompute the scale
    // since it's assumed it doesn't chage once the window has been created. Some render targets
    // might use a different scale, though.
    Real framebufferScale = 1.0f;
    Int32 framebufferWidth = 0;
    Int32 framebufferHeight = 0;
    glfwGetFramebufferSize( m_window, &framebufferWidth, &framebufferHeight );
    if ( enableHDPI && framebufferWidth > 0 ) {
        framebufferScale = Real( framebufferWidth ) / Real( width );
    }

    // Make sure we have proper windows settings defined
    settings->set( "video.width", width );
    settings->set( "video.height", height );
    settings->set( "video.fullscreen", fullscreen );
    settings->set( "video.framebufferScale", framebufferScale );

    CRIMILD_LOG_DEBUG( "Created window with size ", width, "x", height, " (x", framebufferScale, ")" );

    return true;
}

void Window::destroyWindow( void )
{
    glfwSetWindowUserPointer( m_window, nullptr );
    glfwDestroyWindow( m_window );

    CRIMILD_LOG_INFO( "Window destroyed" );
}