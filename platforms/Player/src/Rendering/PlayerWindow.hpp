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

#ifndef CRIMILD_PLAYER_RENDERING_WINDOW_
#define CRIMILD_PLAYER_RENDERING_WINDOW_

#include "Foundation/GLFWUtils.hpp"
#include "Mathematics/Clock.hpp"
#include "Rendering/Extent.hpp"
#include "Simulation/Event.hpp"

#include <string>

namespace crimild {

    namespace vulkan {

        class VulkanInstance;
        class VulkanSurface;
        class PhysicalDevice;
        class RenderDevice;
        class Semaphore;
        class Swapchain;

        namespace framegraph {

            class Node;

        }

    }

    namespace player {

        /**
            \brief Handle window creation
        */
        class Window {
        public:
            struct Options {
                std::string title;
                Extent2D extent = { .width = 800, .height = 600 };
                bool showEditor = false;
                bool fullscreen = false;
                bool enableHDPI = false;
                bool vsync = true;
                bool enableGraphicsSwitching = false;
                float framebufferScale = 1.0f;
            };

        public:
            Window( const Options &options ) noexcept;
            ~Window( void ) noexcept;

            [[nodiscard]] inline GLFWwindow *getHandle( void ) noexcept { return m_window; }

            Event handle( const Event &e ) noexcept;

        public:
            GLFWwindow *getWindowHandler( void ) { return m_window; }

        private:
            bool createWindow( void );
            void registerEventCallbacks( void ) noexcept;
            void destroyWindow( void );

            UInt64 getTimestamp( void ) const noexcept;

        private:
            GLFWwindow *m_window = nullptr;

            std::shared_ptr< vulkan::VulkanInstance > m_instance;
            std::shared_ptr< vulkan::VulkanSurface > m_surface;
            std::shared_ptr< vulkan::PhysicalDevice > m_physicalDevice;
            std::shared_ptr< vulkan::RenderDevice > m_renderDevice;
            std::shared_ptr< vulkan::Swapchain > m_swapchain;

            std::vector< std::shared_ptr< vulkan::Semaphore > > m_imageAvailableSemaphores;
            std::vector< std::shared_ptr< vulkan::Semaphore > > m_renderFinishedSemaphores;
            std::vector< std::shared_ptr< vulkan::framegraph::Node > > m_framegraph;

            Event m_lastResizeEvent = Event {};
            Event m_lastMouseButtonDownEvent = Event {};

            Clock m_clock;

            Options m_options;
        };

    }

}

#endif
