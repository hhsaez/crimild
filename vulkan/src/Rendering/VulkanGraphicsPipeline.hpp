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

#ifndef CRIMILD_VULKAN_RENDERING_GRAPHICS_PIPELINE_
#define CRIMILD_VULKAN_RENDERING_GRAPHICS_PIPELINE_

#include "Foundation/Memory.hpp"
#include "Foundation/VulkanUtils.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/ColorBlendState.hpp"
#include "Rendering/DepthStencilState.hpp"
#include "Rendering/RasterizationState.hpp"
#include "Rendering/VertexLayout.hpp"

namespace crimild {

    class ShaderProgram;

    namespace vulkan {

        class RenderDevice;

        class GraphicsPipeline {
        public:
            struct Descriptor {
                Primitive::Type primitiveType = Primitive::Type::TRIANGLES;
                std::vector< VkDescriptorSetLayout > descriptorSetLayouts;
                const ShaderProgram *program = nullptr;
                std::vector< VertexLayout > vertexLayouts;
                DepthStencilState depthStencilState;
                RasterizationState rasterizationState;
                ColorBlendState colorBlendState;
                size_t colorAttachmentCount = 1;
                std::vector< VkDynamicState > dynamicStates;
                ViewportDimensions viewport;
                ViewportDimensions scissor;
            };

        public:
            GraphicsPipeline(
                RenderDevice *renderDevice,
                VkRenderPass renderPass,
                const Descriptor &descriptor ) noexcept;
            GraphicsPipeline(
                RenderDevice *renderDevice,
                VkRenderPass renderPass,
                const std::vector< VkDescriptorSetLayout > &descriptorSetLayouts,
                const ShaderProgram *program,
                const std::vector< VertexLayout > &vertexLayouts,
                const DepthStencilState &pipelineDepthStencilState = DepthStencilState {},
                const RasterizationState &pipelineRasterizationState = RasterizationState {},
                const ColorBlendState &pipelineColorBlendState = ColorBlendState {},
                size_t colorAttachmentCount = 1,
                std::vector< VkDynamicState > dynamicStates = {} ) noexcept;
            ~GraphicsPipeline( void ) noexcept;

            inline VkPipeline getHandle( void ) const noexcept { return m_pipeline; }
            inline VkPipelineLayout getPipelineLayout( void ) const noexcept { return m_pipelineLayout; }

        private:
            VkDevice m_renderDevice = VK_NULL_HANDLE;
            VkPipeline m_pipeline = VK_NULL_HANDLE;
            VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        };

    }

}

#endif
