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

#ifndef CRIMILD_VULKAN_RENDERING_COMMAND_BUFFER_
#define CRIMILD_VULKAN_RENDERING_COMMAND_BUFFER_

#include "Foundation/VulkanObject.hpp"
#include "Mathematics/Vector.hpp"

namespace crimild {

	namespace vulkan {

        class Buffer;
        class CommandBufferManager;
		class CommandPool;
		class RenderPass;
		class Framebuffer;
		class Pipeline;
        class RenderDevice;

		/**
		 */
		class CommandBuffer : public VulkanObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::CommandBuffer )

		public:
			struct Descriptor {
                RenderDevice *renderDevice;
				CommandPool *commandPool;
			};

			enum class Usage {
				DEFAULT,
				ONE_TIME_SUBMIT,
				RENDER_PASS_CONTINUE,
				SIMULTANEOUS_USE,
			};
			
		public:
			~CommandBuffer( void ) noexcept;
			
            RenderDevice *renderDevice = nullptr;
            CommandPool *commandPool = nullptr;
            VkCommandBuffer handler = VK_NULL_HANDLE;
            CommandBufferManager *manager = nullptr;

        public:
			void begin( Usage usage = Usage::DEFAULT ) const noexcept;
			void beginRenderPass( const RenderPass *renderPass, const Framebuffer *framebuffer, const RGBAColorf &clearColor ) const noexcept;
			void bindGraphicsPipeline( const Pipeline *pipeline ) const noexcept;
            void bindVertexBuffer( const Buffer *buffer ) const noexcept;
			void draw( crimild::UInt32 vertexCount ) const noexcept;
			void endRenderPass( void ) const noexcept;
			void end( void ) const;
		};

        class CommandBufferManager : public VulkanObjectManager< CommandBuffer > {
        public:
            explicit CommandBufferManager( RenderDevice *renderDevice = nullptr ) noexcept : m_renderDevice( renderDevice ) { }
            virtual ~CommandBufferManager( void ) noexcept = default;

            SharedPointer< CommandBuffer > create( CommandBuffer::Descriptor const &descriptor ) noexcept;
            void destroy( CommandBuffer *commandBuffer ) noexcept override;

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

	}

}
	
#endif
	
