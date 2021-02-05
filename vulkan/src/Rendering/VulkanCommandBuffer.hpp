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

#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/VulkanRenderResource.hpp"

namespace crimild {

    class Buffer;
    class DescriptorSet;
    class Framebuffer;
    class GraphicsPipeline;
    class ComputePipeline;
    class RenderPass;

    namespace vulkan {

        struct CommandBufferBindInfo {
            VkCommandBuffer handler;
        };

        class CommandBufferManager : public BasicRenderResourceManagerImpl< CommandBuffer, CommandBufferBindInfo > {
            using ManagerImpl = BasicRenderResourceManagerImpl< CommandBuffer, CommandBufferBindInfo >;

        public:
            virtual ~CommandBufferManager( void ) noexcept = default;

            crimild::Bool bind( CommandBuffer *commandBuffer ) noexcept override;
            crimild::Bool unbind( CommandBuffer *commandBuffer ) noexcept override;

            void updateCommandBuffer( CommandBuffer *commandBuffer ) noexcept;

        private:
            void recordCommands( RenderDevice *renderDevice, CommandBuffer *parent, CommandBuffer *commandBuffer ) noexcept;

        private:
            GraphicsPipeline *m_currentGraphicsPipeline = nullptr;
            ComputePipeline *m_currentComputePipeline = nullptr;
            SharedPointer< Framebuffer > m_currentFramebuffer;
            SharedPointer< RenderPass > m_currentRenderPass;
            crimild::UInt32 m_boundDescriptorSets = 0;
            crimild::Size m_vertexOffset = 0;
            crimild::Size m_indexOffset = 0;
        };

    }

}

#endif
