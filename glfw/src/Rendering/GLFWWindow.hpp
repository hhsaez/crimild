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

#ifndef CRIMILD_GLFW_RENDERING_WINDOW_
#define CRIMILD_GLFW_RENDERING_WINDOW_

#include "Foundation/GLFWUtils.hpp"
#include "Rendering/GLFWVulkanSurface.hpp"
#include "Rendering/VulkanInstance.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Simulation/Event.hpp"

#include <string>

namespace crimild {

    namespace vulkan {

        class RenderDevice;

        class RenderPass {
        public:
            virtual ~RenderPass( void ) = default;

            virtual void render( RenderDevice *renderDevice ) noexcept = 0;
        };

    }

    namespace glfw {

        /**
		   \brief Handle window creation
		 */
        class Window {
        public:
            Window( void ) noexcept;
            ~Window( void ) noexcept;

            [[nodiscard]] inline GLFWwindow *getHandle( void ) noexcept { return m_window; }

            Event handle( const Event &e ) noexcept;

        public:
            GLFWwindow *getWindowHandler( void ) { return m_window; }

        private:
            bool createWindow( void );
            void destroyWindow( void );

        private:
            GLFWwindow *m_window = nullptr;

            // TODO: For a multi-window setup, we should move the Vulkan instance outside of this class
            std::unique_ptr< vulkan::VulkanInstance > m_instance;

            // I think it's ok to have one surface per window
            std::unique_ptr< vulkan::VulkanSurface > m_surface;

            // TODO: For a multi-device setup, we should move the physical device outside of this class
            std::unique_ptr< vulkan::PhysicalDevice > m_physicalDevice;

            std::unique_ptr< vulkan::RenderDevice > m_renderDevice;

            std::unique_ptr< vulkan::RenderPass > m_renderPass;
        };

    }

}

#endif
