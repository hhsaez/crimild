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

#ifndef CRIMILD_VULKAN_RENDERING_FRAMEBUFFER_
#define CRIMILD_VULKAN_RENDERING_FRAMEBUFFER_

#include "Foundation/VulkanObject.hpp"
#include "Foundation/Containers/Array.hpp"

namespace crimild {

	namespace vulkan {

        class FramebufferManager;
		class ImageView;
        class RenderDevice;
        class RenderPass;

		/**
		 */
		class Framebuffer : public VulkanObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::Framebuffer )

		public:
			struct Descriptor {
                RenderDevice *renderDevice;
				std::vector< ImageView * > attachments;
				RenderPass *renderPass;
				VkExtent2D extent;
			};

		public:
			~Framebuffer( void ) noexcept;

            RenderDevice *renderDevice = nullptr;
            VkFramebuffer handler = nullptr;
            VkExtent2D extent;
            FramebufferManager *manager = nullptr;
		};

        class FramebufferManager : public VulkanObjectManager< Framebuffer > {
        public:
            explicit FramebufferManager( RenderDevice *renderDevice = nullptr ) noexcept : m_renderDevice( renderDevice ) { }
            virtual ~FramebufferManager( void ) noexcept = default;

            Framebuffer *getFramebuffer( crimild::Size index ) noexcept
            {
                return m_framebuffers[ index ];
            }

            SharedPointer< Framebuffer > create( Framebuffer::Descriptor const &descriptor ) noexcept;
            void destroy( Framebuffer *framebuffer ) noexcept override;

        private:
            RenderDevice *m_renderDevice = nullptr;
            containers::Array< Framebuffer * > m_framebuffers;
        };

	}

}
	
#endif
	
