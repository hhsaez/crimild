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

#include "Editor/EditorLayer.hpp"
#include "Foundation/VulkanUtils.hpp"
#include "Rendering/ImGUILayer.hpp"
#include "Rendering/Layer.hpp"
#include "Rendering/RenderPasses/VulkanClearPass.hpp"
#include "Rendering/RenderPasses/VulkanPresentPass.hpp"
#include "Rendering/RenderPasses/VulkanScenePass.hpp"
#include "Rendering/RenderPasses/VulkanSelectionOutlinePass.hpp"
#include "Rendering/RenderPasses/VulkanShaderPass.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/SimulationLayer.hpp"

#include <array>
#include <iomanip>

using namespace crimild;
using namespace crimild::glfw;

class MainLayer : public Layer {
public:
    explicit MainLayer( vulkan::RenderDevice *renderDevice ) noexcept
        : m_renderDevice( renderDevice ),
          m_uniforms( crimild::alloc< UniformBuffer >( Vector2 { 1024, 768 } ) )
    {
        init();

        m_clear = crimild::alloc< vulkan::ClearPass >(
            m_renderDevice,
            std::vector< const vulkan::FramebufferAttachment * > {
                &m_colorAttachment,
                // &m_depthAttachment,
            }
        );

        m_shader = crimild::alloc< vulkan::ShaderPass >(
            renderDevice,
            R"(
                layout ( set = 0, binding = 0 ) uniform Context {
                    vec2 dimensions;
                } context;

                float circleMask( vec2 uv, vec2 p, float r, float blur )
                {
                    float d = length( uv - p );
                    return d > r ? 0.0 : 1.0;
                }

                void main()
                {
                    vec2 uv = inTexCoord;
                    uv -= 0.5;
                    uv.x *= context.dimensions.x / context.dimensions.y;

                    float blur = 0.00625;

                    float mask = circleMask( uv, vec2( 0.0 ), 0.4, blur );
                    mask -= circleMask( uv, vec2( 0.0, 0.0 ), 0.25, blur );
                    if ( uv.x > 0.2 ) mask = 0.0;
                    vec3 faceColor = vec3( 0.3, 0.3, 0.3 ) * mask;

                    vec3 color = faceColor;

                    outColor = vec4( color, 1.0 );
                }
            )",
            &m_colorAttachment,
            m_uniforms
        );

        m_present = crimild::alloc< vulkan::PresentPass >( m_renderDevice, &m_colorAttachment );
    }

    virtual ~MainLayer( void ) noexcept
    {
        m_clear = nullptr;
        m_shader = nullptr;
        m_present = nullptr;

        deinit();
    }

    inline vulkan::RenderDevice *getRenderDevice( void ) noexcept { return m_renderDevice; }

    virtual Event handle( const Event &e ) noexcept override
    {
        if ( e.type == Event::Type::WINDOW_RESIZE ) {
            // Recreate attachments since swapchain was recreated too.
            deinit();
            init();
        }

        m_clear->handle( e );
        m_shader->handle( e );
        m_present->handle( e );

        return Layer::handle( e );
    }

    virtual void render( void ) noexcept override
    {
        m_clear->render();
        m_shader->render();

        Layer::render();

        m_present->render();
    }

private:
    void init( void ) noexcept
    {
        const auto extent = getRenderDevice()->getSwapchainExtent();

        m_uniforms->setValue( Vector2 { float( extent.width ), float( extent.height ) } );

        getRenderDevice()->createFramebufferAttachment( "Swapchain/Color", extent, getRenderDevice()->getSwapchainFormat(), m_colorAttachment, true );
        getRenderDevice()->createFramebufferAttachment( "Swapchain/Depth", extent, getRenderDevice()->getDepthStencilFormat(), m_depthAttachment, true );
    }

    void deinit( void ) noexcept
    {
        getRenderDevice()->destroyFramebufferAttachment( m_colorAttachment );
        getRenderDevice()->destroyFramebufferAttachment( m_depthAttachment );
    }

private:
    vulkan::RenderDevice *m_renderDevice = nullptr;

    vulkan::FramebufferAttachment m_colorAttachment;
    vulkan::FramebufferAttachment m_depthAttachment;

    SharedPointer< vulkan::ClearPass > m_clear;
    SharedPointer< vulkan::ShaderPass > m_shader;
    SharedPointer< vulkan::PresentPass > m_present;

    SharedPointer< UniformBuffer > m_uniforms;
};

Window::Window( const Options &options ) noexcept
    : m_options( options )
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

    m_mainLayer = std::make_unique< MainLayer >( m_renderDevice.get() );

    if ( m_options.showEditor ) {
        m_mainLayer
            ->attach< ImGUILayer >(
                m_renderDevice.get(),
                m_options.extent,
                m_options.framebufferScale
            )
            ->attach< EditorLayer >( m_renderDevice.get() );
    } else {
        m_mainLayer->attach< SimulationLayer >( m_renderDevice.get() );
    }

    // Set last resize event to the initial window's size so it will be forwarded
    // to render passes during the next update.
    // TODO: This re-creates many rendering objects in many passes. It would be ideal
    // to avoid it and just initialize all passes correctly
    m_lastResizeEvent = Event {
        .type = Event::Type::WINDOW_RESIZE,
        .extent = options.extent,
    };
}

Window::~Window( void ) noexcept
{
    if ( m_renderDevice != nullptr ) {
        m_renderDevice->flush();
    }

    m_mainLayer = nullptr;

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
                return Event { .type = Event::Type::WINDOW_CLOSED };
            }

            if ( Input::getInstance() != nullptr ) {
                switch ( Input::getInstance()->getMouseCursorMode() ) {
                    case Input::MouseCursorMode::NORMAL:
                        glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
                        break;

                    case Input::MouseCursorMode::HIDDEN:
                        glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
                        break;

                    case Input::MouseCursorMode::GRAB:
                        glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
                        break;

                    default:
                        break;
                }
            }

            auto ret = m_mainLayer->handle( e );
            if ( ret.type == Event::Type::TERMINATE ) {
                return ret;
            }

            if ( m_lastResizeEvent.type != Event::Type::NONE ) {
                m_renderDevice->handle( m_lastResizeEvent );
                m_mainLayer->handle( m_lastResizeEvent );
                m_lastResizeEvent = Event {};
            }

            if ( m_renderDevice->beginRender() ) {
                m_mainLayer->render();
                m_renderDevice->endRender();
            }

            m_clock.tick();

            break;
        }

        case Event::Type::WINDOW_RESIZE: {
            CRIMILD_LOG_DEBUG( "Window resized to ", e.extent.width, "x", e.extent.height );
            m_options.extent = e.extent;

            // Delay event until next render, so we only forward it once after the resizing is completed
            m_lastResizeEvent = e;
            break;
        }

        default: {
            m_renderDevice->handle( e );
            m_mainLayer->handle( e );
            break;
        }
    }

    return e;
}

bool Window::createWindow( void )
{
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

    glfwWindowHint( GLFW_COCOA_RETINA_FRAMEBUFFER, m_options.enableHDPI ? GLFW_TRUE : GLFW_FALSE );
    glfwWindowHint( GLFW_COCOA_GRAPHICS_SWITCHING, m_options.enableGraphicsSwitching ? GLFW_TRUE : GLFW_FALSE );

    m_window = glfwCreateWindow(
        m_options.extent.width,
        m_options.extent.height,
        m_options.showEditor ? "Crimild Editor" : m_options.title.c_str(),
        m_options.fullscreen ? glfwGetPrimaryMonitor() : NULL,
        NULL
    );
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
            if ( window != nullptr && windowHandle == window->m_window ) {
                const auto extent = Extent2D { .width = Real( width ), .height = Real( height ) };
                window->m_options.extent = extent;
                window->handle(
                    Event {
                        .type = Event::Type::WINDOW_RESIZE,
                        .extent = extent,
                    }
                );
            } else {
                CRIMILD_LOG_WARNING( "glfwSetWindowSizeCallback: GLFW window user pointer not set" );
            }
        }
    );

#if !defined( CRIMILD_ENABLE_VULKAN )
    glfwMakeContextCurrent( _window );
    // auto vsync = settings->get< crimild::Bool >( "video.vsync", true );
    glfwSwapInterval( m_options.vsync ? 1 : 0 );
#endif

    // Compute framebuffer scale
    // This is doen only once. Resizing the window does not require to recompute the scale
    // since it's assumed it doesn't chage once the window has been created. Some render targets
    // might use a different scale, though.
    Int32 framebufferWidth = 0;
    Int32 framebufferHeight = 0;
    glfwGetFramebufferSize( m_window, &framebufferWidth, &framebufferHeight );
    if ( m_options.enableHDPI && framebufferWidth > 0 ) {
        m_options.framebufferScale = Real( framebufferWidth ) / Real( m_options.extent.width );
    }

    CRIMILD_LOG_DEBUG(
        "Created window with size ",
        m_options.extent.width,
        "x",
        m_options.extent.height,
        " (x",
        m_options.framebufferScale,
        ")"
    );

    registerEventCallbacks();

    return true;
}

void Window::registerEventCallbacks( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    glfwSetKeyCallback(
        m_window,
        []( GLFWwindow *windowHandle, int key, int scancode, int action, int mod ) {
            if ( key == GLFW_KEY_UNKNOWN ) {
                CRIMILD_LOG_WARNING( "Unknown key pressed (", key, ")" );
                return;
            }

            auto window = static_cast< Window * >( glfwGetWindowUserPointer( windowHandle ) );
            if ( window != nullptr && windowHandle == window->m_window ) {
                window->handle(
                    Event {
                        .type = [ & ] {
                            switch ( action ) {
                                case GLFW_PRESS:
                                    return Event::Type::KEY_DOWN;
                                case GLFW_REPEAT:
                                    return Event::Type::KEY_REPEAT;
                                case GLFW_RELEASE:
                                default:
                                    return Event::Type::KEY_UP;
                            }
                        }(),
                        .timestamp = window->getTimestamp(),
                        .keyboard = Keyboard {
                            .state = [ & ] {
                                switch ( action ) {
                                    case GLFW_PRESS:
                                        return Keyboard::State::PRESSED;
                                    case GLFW_REPEAT:
                                        return Keyboard::State::REPEAT;
                                    case GLFW_RELEASE:
                                    default:
                                        return Keyboard::State::RELEASED;
                                }
                            }(),
                            .key = UInt32( key ),
                            .scancode = UInt32( scancode ),
                            .mod = UInt32( mod ),
                        },
                    }
                );
            }
        }
    );

    glfwSetCharCallback(
        m_window,
        []( GLFWwindow *windowHandle, unsigned int codepoint ) {
            auto window = static_cast< Window * >( glfwGetWindowUserPointer( windowHandle ) );
            if ( window != nullptr && windowHandle == window->m_window ) {
                window->handle(
                    Event {
                        .type = Event::Type::TEXT,
                        .timestamp = window->getTimestamp(),
                        .text = {
                            .codepoint = codepoint,
                        },
                    }
                );
            }
        }
    );

    glfwSetMouseButtonCallback(
        m_window,
        []( GLFWwindow *windowHandle, int button, int action, int mods ) {
            auto window = static_cast< Window * >( glfwGetWindowUserPointer( windowHandle ) );
            if ( window != nullptr && windowHandle == window->m_window ) {
                double x, y;
                glfwGetCursorPos( window->m_window, &x, &y );

                const auto ts = window->getTimestamp();

                const auto e = Event {
                    .type = action == GLFW_PRESS ? Event::Type::MOUSE_BUTTON_DOWN : Event::Type::MOUSE_BUTTON_UP,
                    .timestamp = ts,
                    .button = MouseButton {
                        .button = UInt8( button ),
                        .state = action == GLFW_PRESS ? MouseButton::State::PRESSED : MouseButton::State::RELEASED,
                        .pos = Vector2i {
                            Int( std::floor( x ) ),
                            Int( std::floor( y ) ),
                        },
                        .npos = Vector2f {
                            Real( x / window->m_options.extent.width ),
                            Real( y / window->m_options.extent.height ),
                        },
                    },
                };

                window->handle( e );

                if ( action == GLFW_PRESS ) {
                    window->m_lastMouseButtonDownEvent = e;
                } else if ( action == GLFW_RELEASE ) {
                    if ( e.timestamp - window->m_lastMouseButtonDownEvent.timestamp < 500 ) {
                        window->handle(
                            Event {
                                .type = Event::Type::MOUSE_CLICK,
                                .timestamp = ts,
                                .button = MouseButton {
                                    .button = UInt8( button ),
                                    .state = MouseButton::State::RELEASED,
                                    .pos = Vector2i {
                                        Int( std::floor( x ) ),
                                        Int( std::floor( y ) ),
                                    },
                                    .npos = Vector2f {
                                        Real( x / window->m_options.extent.width ),
                                        Real( y / window->m_options.extent.height ),
                                    },
                                },
                            }
                        );
                    }
                }
            }
        }
    );

    // Required in order to receive mouse motion events when cursor is disabled
    if ( glfwRawMouseMotionSupported() ) {
        glfwSetInputMode( m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE );
    }

    glfwSetCursorPosCallback(
        m_window,
        []( GLFWwindow *windowHandle, double xpos, double ypos ) {
            auto window = static_cast< Window * >( glfwGetWindowUserPointer( windowHandle ) );
            if ( window != nullptr && windowHandle == window->m_window ) {
                window->handle(
                    Event {
                        .type = Event::Type::MOUSE_MOTION,
                        .timestamp = window->getTimestamp(),
                        .motion = MouseMotion {
                            .pos = Vector2i {
                                Int( std::floor( xpos ) ),
                                Int( std::floor( ypos ) ),
                            },
                            .npos = Vector2f {
                                Real( xpos / window->m_options.extent.width ),
                                Real( ypos / window->m_options.extent.height ),
                            },
                        },
                    }
                );
            }
        }
    );

    glfwSetScrollCallback(
        m_window,
        []( GLFWwindow *windowHandle, double xoffset, double yoffset ) {
            auto window = static_cast< Window * >( glfwGetWindowUserPointer( windowHandle ) );
            if ( window != nullptr && windowHandle == window->m_window ) {
                window->handle(
                    Event {
                        .type = Event::Type::MOUSE_WHEEL,
                        .timestamp = window->getTimestamp(),
                        .wheel = MouseWheel {
                            .x = Int32( std::floor( xoffset ) ),
                            .y = Int32( std::floor( yoffset ) ),
                        } }
                );
            }
        }
    );
}

void Window::destroyWindow( void )
{
    glfwSetWindowUserPointer( m_window, nullptr );
    glfwDestroyWindow( m_window );

    CRIMILD_LOG_INFO( "Window destroyed" );
}

UInt64 Window::getTimestamp( void ) const noexcept
{
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast< std::chrono::milliseconds >( now ).count();
}
