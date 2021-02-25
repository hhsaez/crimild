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

#include "Foundation/Containers/Map.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/VulkanRenderResource.hpp"
#include "Rendering/VulkanShaderCompiler.hpp"
#include "Rendering/VulkanShaderModule.hpp"

namespace crimild {

    class DescriptorSetLayout;
    class Shader;
    class ShaderProgram;

    namespace vulkan {

        class PipelineLayout;

        struct GraphicsPipelineBindInfo {
            VkPipeline pipelineHandler = VK_NULL_HANDLE;
            SharedPointer< PipelineLayout > pipelineLayout;
        };

        /**
			\TODO: GraphicsPipelines might be bound by render pass. If so, pipelines might have multiple handlers
         	(one for each render pass)
         */
        class GraphicsPipelineManager : public BasicRenderResourceManagerImpl< GraphicsPipeline, GraphicsPipelineBindInfo > {
            using ManagerImpl = BasicRenderResourceManagerImpl< GraphicsPipeline, GraphicsPipelineBindInfo >;

        public:
            virtual ~GraphicsPipelineManager( void ) noexcept = default;

            crimild::Bool bind( GraphicsPipeline *graphicsPipeline ) noexcept override;
            crimild::Bool unbind( GraphicsPipeline *graphicsPipeline ) noexcept override;

        private:
            using ShaderModuleArray = std::vector< SharedPointer< ShaderModule > >;
            using ShaderStageArray = std::vector< VkPipelineShaderStageCreateInfo >;

            ShaderModuleArray createShaderModules( RenderDevice *renderDevice, ShaderProgram *program ) const noexcept;
            ShaderStageArray createShaderStages( const ShaderModuleArray &modules ) const noexcept;
            VkPipelineShaderStageCreateInfo createShaderStage( const ShaderModule &module ) const noexcept;
            Array< VkVertexInputBindingDescription > getVertexInputBindingDescriptions( GraphicsPipeline *graphicsPipeline ) const noexcept;
            Array< VkVertexInputAttributeDescription > getVertexInputAttributeDescriptions( RenderDevice *renderDevice, GraphicsPipeline *graphicsPipeline ) const noexcept;
            VkPipelineVertexInputStateCreateInfo createVertexInput( const Array< VkVertexInputBindingDescription > &bindingDescriptions, const Array< VkVertexInputAttributeDescription > &attributeDescriptions ) const noexcept;
            VkPipelineInputAssemblyStateCreateInfo createInputAssemby( Primitive::Type primitiveType ) const noexcept;
            VkViewport createViewport( const ViewportDimensions &viewport ) const noexcept;
            VkRect2D createScissor( const ViewportDimensions &scissor ) const noexcept;
            VkPipelineViewportStateCreateInfo createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) const noexcept;
            VkPipelineViewportStateCreateInfo createDynamicViewportState( crimild::Bool hasViewport, crimild::Bool hasScissor ) const noexcept;
            VkPipelineRasterizationStateCreateInfo createRasterizer( GraphicsPipeline *graphicsPipeline ) const noexcept;
            VkPipelineMultisampleStateCreateInfo createMultiplesampleState( void ) const noexcept;
            VkPipelineDepthStencilStateCreateInfo createDepthStencilState( GraphicsPipeline *graphicsPipeline ) const noexcept;
            VkPipelineColorBlendAttachmentState createColorBlendAttachment( GraphicsPipeline *graphicsPipeline ) const noexcept;
            VkPipelineColorBlendStateCreateInfo createColorBlending( const Array< VkPipelineColorBlendAttachmentState > &colorBlendAttachments ) const noexcept;

            using DynamicStates = std::vector< VkDynamicState >;
            DynamicStates getDynamicStates( GraphicsPipeline *graphicsPipeline ) const noexcept;
            VkPipelineDynamicStateCreateInfo createDynamicState( DynamicStates &dynamicStates ) const noexcept;
        };

    }

}

#endif
