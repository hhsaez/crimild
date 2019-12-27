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
*     * Neither the name of the copyright holders nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CRIMILD_VULKAN_RENDERING_INSTANCE_
#define CRIMILD_VULKAN_RENDERING_INSTANCE_

#include "VulkanPhysicalDevice.hpp"
#include "Debug/VulkanDebugMessenger.hpp"
#include "Foundation/VulkanObject.hpp"

#include <vector>

namespace crimild {

	namespace vulkan {

        class VulkanInstanceManager;
		class VulkanSurface;
		class VulkanRenderDevice;
		class Swapchain;

		/**
		   \brief Handles creation and setup for the Vulkan instance
		 */
        class VulkanInstance : public VulkanObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::VulkanInstance )

        public:
            struct Descriptor {
                std::string appName;
                crimild::UInt32 appVersionMajor;
                crimild::UInt32 appVersionMinor;
                crimild::UInt32 appVersionPatch;
            };

        public:
			~VulkanInstance( void );

            VkInstance handler = VK_NULL_HANDLE;

            // This may be confusing. Whoever creates the instance must be derived
            // from VulkanInstanceManager. This is so because the instance does not
            // have a true parent object. Again, confusing, but also keeps it
            // standardized with other objects
            VulkanInstanceManager *manager = nullptr;

			/**
			   \name Surface
			 */
			//@{

//		public:
//			void setSurface( SharedPointer< VulkanSurface > const &surface ) noexcept { m_surface = surface; }
//			VulkanSurface *getSurface( void ) noexcept { return crimild::get_ptr( m_surface ); }

//		private:
//			SharedPointer< VulkanSurface > m_surface;

			//@}

			/**
			   \name Render Device			   
			 */
			//@{

//		public:
//			void setRenderDevice( SharedPointer< VulkanRenderDevice > const &renderDevice ) noexcept { m_renderDevice = renderDevice; }
//			VulkanRenderDevice *getRenderDevice( void ) noexcept { return crimild::get_ptr( m_renderDevice ); }

//		private:
//			SharedPointer< VulkanRenderDevice > m_renderDevice;

			//@}
		};

        class VulkanInstanceManager : public VulkanObjectManager< VulkanInstance > {
        public:
            virtual ~VulkanInstanceManager( void ) = default;

            SharedPointer< VulkanInstance > create( VulkanInstance::Descriptor const &descriptor ) noexcept;
            void destroy( VulkanInstance *instance ) noexcept override;
        };

	}

}
	
#endif
	
