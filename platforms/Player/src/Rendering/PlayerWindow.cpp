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

#include "Rendering/PlayerWindow.hpp"

#include "Rendering/FrameGraph/VulkanRenderScene.hpp"
#include "Rendering/PlayerVulkanSurface.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanInstance.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderDeviceCache.hpp"
#include "Rendering/VulkanSwapchain.hpp"
#include "Simulation/Settings.hpp"

using namespace crimild;
using namespace crimild::player;

Window::Window( const Options &options ) noexcept
    : m_options( options )
{
    if ( !createWindow() ) {
        exit( -1 );
    }

    // The instance should be independent from the window. What if we have multiple windows?
    m_instance = std::make_shared< vulkan::VulkanInstance >();

    // TODO: I think the surface should be created by the instance
    m_surface = std::make_shared< player::VulkanSurface >( m_instance.get(), getHandle() );

    // The physical device should be independent from the window. What if we have multiple windows?
    m_physicalDevice = m_instance->createPhysicalDevice( m_surface.get() );

    m_renderDevice = m_physicalDevice->createRenderDevice();

    m_swapchain = crimild::alloc< vulkan::Swapchain >(
        m_renderDevice.get(),
        m_surface.get(),
        VkExtent2D {
            ( uint32_t ) options.extent.width,
            ( uint32_t ) options.extent.height,
        }
    );
    // Set frame count to be one more than the swapchain available images
    // That way we'll have enough room for double or triple buffering.
    m_renderDevice->configure( m_swapchain->getImageCount() );

    const auto extent = VkExtent2D {
        // Render scene using a fixed aspect ratio, so we don't need to
        // resize images when windows does. The end result might be
        // a bit pixelated, but it's faster. And we can always render to
        // a bigger buffer
        .width = 1280,
        .height = 720,
    };

    m_imageAvailableSemaphores.resize( m_renderDevice->getInFlightFrameCount() );
    m_renderFinishedSemaphores.resize( m_renderDevice->getInFlightFrameCount() );
    m_framegraph.resize( m_renderDevice->getInFlightFrameCount() );
    for ( int i = 0; i < m_renderDevice->getInFlightFrameCount(); ++i ) {
        m_imageAvailableSemaphores[ i ] = crimild::alloc< vulkan::Semaphore >(
            m_renderDevice.get(),
            "ImageAvailable",
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );
        m_renderFinishedSemaphores[ i ] = crimild::alloc< vulkan::Semaphore >(
            m_renderDevice.get(),
            "RenderFinished",
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );

        m_framegraph[ i ] = crimild::alloc< crimild::vulkan::framegraph::RenderScene >(
            m_renderDevice.get(),
            "Scene",
            nullptr,
            m_swapchain->getImageView( i ), // renderSceneOutput,
            crimild::vulkan::SyncOptions {
                .pre = {
                    .imageMemoryBarriers = {
                        crimild::vulkan::ImageMemoryBarrier {
                            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                            .srcAccessMask = 0,
                            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                            .imageView = m_swapchain->getImageView( i ),
                        },
                    },
                },
                .post = {
                    .imageMemoryBarriers = {
                        crimild::vulkan::ImageMemoryBarrier {
                            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                            .dstAccessMask = 0,
                            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                            .imageView = m_swapchain->getImageView( i ),
                        },
                    },
                },
                .wait = { m_imageAvailableSemaphores[ i ] },
                .signal = { m_renderFinishedSemaphores[ i ] },
            }
        );
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

    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_framegraph.clear();
    m_swapchain = nullptr;
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

            if ( m_lastResizeEvent.type != Event::Type::NONE ) {
                // TODO: handle resize
                m_lastResizeEvent = Event {};
            }

            auto currentFrame = ( m_renderDevice->getCurrentFrameIndex() + 1 ) % m_renderDevice->getInFlightFrameCount();
            m_renderDevice->setCurrentFrameIndex( currentFrame );

            uint32_t imageIndex;
            bool canPresent = m_swapchain->acquireNextImage(
                imageIndex,
                {
                    .signal = {
                        m_imageAvailableSemaphores[ currentFrame ],
                    },
                }
            );
            if ( canPresent ) {
                m_renderDevice->getCache()->onBeforeFrame();
                m_framegraph[ currentFrame ]->execute();
                m_renderDevice->getCache()->onAfterFrame();
                m_swapchain->present(
                    imageIndex,
                    {
                        .wait = {
                            m_renderFinishedSemaphores[ currentFrame ],
                        },
                    }
                );
            } else {
                // TODO: handle swapchain recreation due to invalid size
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
            break;
        }
    }

    return e;
}

bool Window::createWindow( void )
{
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
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
