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

#include "VulkanPipeline.hpp"
#include "VulkanRenderDevice.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool PipelineManager::bind( Pipeline *pipeline ) noexcept
{
    if ( validate( pipeline ) ) {
        // Pipeline already bound
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan pripeline" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        return false;
    }

    // WARNING: all of these config params are used when creating the pipeline and
    // they must be alive when vkCreatePipeline is called. Beware of scopes!
    auto shaderModules = createShaderModules( renderDevice, crimild::get_ptr( pipeline->program ) );
    auto shaderStages = createShaderStages( shaderModules );
    auto vertexBindingDescriptions = getVertexInputBindingDescriptions( pipeline );
    auto vertexAttributeDescriptions = getVertexInputAttributeDescriptions( pipeline );
    auto vertexInputInfo = createVertexInput( vertexBindingDescriptions, vertexAttributeDescriptions );
    auto inputAssembly = createInputAssemby( pipeline->primitiveType );
    auto viewport = createViewport( pipeline->viewport );
    auto scissor = createScissor( pipeline->scissor );
    auto viewportState = createViewportState( viewport, scissor );
    auto rasterizer = createRasterizer();
    auto multisampleState = createMultiplesampleState();
    auto depthStencilState = createDepthStencilState();
    auto colorBlendAttachment = createColorBlendAttachment();
    auto colorBlending = createColorBlending( colorBlendAttachment );

    auto pipelineLayout = renderDevice->create(
        PipelineLayout::Descriptor {
            .setLayouts = { crimild::get_ptr( pipeline->descriptorSetLayout ) },
        }
    );

    auto renderPass = renderDevice->getRenderPass();

    auto createInfo = VkGraphicsPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast< uint32_t >( shaderStages.size() ),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = nullptr, // Optional
        .pColorBlendState = &colorBlending,
        .pDynamicState = nullptr, // Optional
        .layout = pipelineLayout->handler,
        .renderPass = renderPass->handler,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE, // Optional
        .basePipelineIndex = -1, // Optional
    };

    VkPipeline pipelineHander;
    CRIMILD_VULKAN_CHECK(
 		vkCreateGraphicsPipelines(
            renderDevice->handler,
           	VK_NULL_HANDLE,
           	1,
           	&createInfo,
           	nullptr,
           	&pipelineHander
       	)
    );

    setHandler( pipeline, pipelineHander );

    m_pipelineLayouts[ pipeline ] = pipelineLayout;

    return ManagerImpl::bind( pipeline );
}

crimild::Bool PipelineManager::unbind( Pipeline *pipeline ) noexcept
{
    if ( !validate( pipeline ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbind Vulkan pipeline" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto handler = getHandler( pipeline );

    vkDestroyPipeline(
        renderDevice->handler,
        handler,
        nullptr
    );

    removeHandlers( pipeline );

    m_pipelineLayouts.remove( pipeline );

    return ManagerImpl::unbind( pipeline );
}

PipelineManager::ShaderModuleArray PipelineManager::createShaderModules( RenderDevice *renderDevice, ShaderProgram *program ) const noexcept
{
    CRIMILD_LOG_TRACE( "Creating shader modules" );

    ShaderModuleArray modules;
    program->getShaders().each( [ &modules, renderDevice ]( SharedPointer< Shader > &shader ) {
        auto module = renderDevice->create(
			ShaderModule::Descriptor {
        		.shader = crimild::get_ptr( shader )
        	}
       	);
        if ( module != nullptr ) {
        	modules.push_back( module );
        }
    });

    return modules;
}

PipelineManager::ShaderStageArray PipelineManager::createShaderStages( const ShaderModuleArray &modules ) const noexcept
{
    CRIMILD_LOG_TRACE( "Creating shader stages" );

    ShaderStageArray shaderStages;
    for ( const auto &module : modules ) {
        auto stage = VkPipelineShaderStageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = module->stage,
            .module = module->handler,
            .pName = module->entryPointName.c_str(),
        };
        shaderStages.push_back( stage );
    }
    return shaderStages;
}

std::vector< VkVertexInputBindingDescription > PipelineManager::getVertexInputBindingDescriptions( Pipeline *pipeline ) const noexcept
{
    return std::vector< VkVertexInputBindingDescription > {
        [ pipeline ] {
            return VkVertexInputBindingDescription {
                .binding = pipeline->bindingDescription.binding,
                .stride = pipeline->bindingDescription.stride,
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            };
        }(),
    };
}

std::vector< VkVertexInputAttributeDescription > PipelineManager::getVertexInputAttributeDescriptions( Pipeline *pipeline ) const noexcept
{
    std::vector< VkVertexInputAttributeDescription > attributeDescriptions;
    for ( auto &attr : pipeline->attributeDescriptions ) {
        attributeDescriptions.push_back( [ &attr ] {
            auto format = VK_FORMAT_UNDEFINED;
            switch ( attr.format ) {
                case VertexInputAttributeDescription::Format::R32: {
                    format = VK_FORMAT_R32_SFLOAT;
                    break;
                }
                case VertexInputAttributeDescription::Format::R32G32: {
                    format = VK_FORMAT_R32G32_SFLOAT;
                    break;
                }
                case VertexInputAttributeDescription::Format::R32G32B32: {
                    format = VK_FORMAT_R32G32B32_SFLOAT;
                    break;
                }
                case VertexInputAttributeDescription::Format::R32G32B32A32: {
                    format = VK_FORMAT_R32G32B32A32_SFLOAT;
                    break;
                }
                default:
                    break;
            }
            return VkVertexInputAttributeDescription {
                .binding = attr.binding,
                .location = attr.location,
                .format = format,
                .offset = attr.offset,
            };
        }());
    }

    return attributeDescriptions;
}

VkPipelineVertexInputStateCreateInfo PipelineManager::createVertexInput( const std::vector< VkVertexInputBindingDescription > &bindingDescriptions, const std::vector< VkVertexInputAttributeDescription > &attributeDescriptions ) const noexcept
{
    return VkPipelineVertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = static_cast< crimild::UInt32 >( bindingDescriptions.size() ),
        .pVertexBindingDescriptions = bindingDescriptions.data(),
        .vertexAttributeDescriptionCount = static_cast< crimild::UInt32 >( attributeDescriptions.size() ),
        .pVertexAttributeDescriptions = attributeDescriptions.data(),
    };
}

VkPipelineInputAssemblyStateCreateInfo PipelineManager::createInputAssemby( Primitive::Type primitiveType ) const noexcept
{
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32 restartEnable = VK_FALSE;

    switch ( primitiveType ) {
        case Primitive::Type::POINTS:
            topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        case Primitive::Type::LINES:
            topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case Primitive::Type::LINE_LOOP:
            topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            restartEnable = VK_TRUE;
            break;
        case Primitive::Type::LINE_STRIP:
            topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        case Primitive::Type::TRIANGLES:
            topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case Primitive::Type::TRIANGLE_STRIP:
            topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        case Primitive::Type::TRIANGLE_FAN:
            topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
            restartEnable = VK_TRUE;
            break;
        default:
            break;
    }

    return VkPipelineInputAssemblyStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = topology,
        .primitiveRestartEnable = restartEnable,
    };
}

VkViewport PipelineManager::createViewport( const Rectf &viewport ) const noexcept
{
    return VkViewport {
        .x = viewport.getX(),
        .y = viewport.getY(),
        .width = viewport.getWidth(),
        .height = viewport.getHeight(),
        .minDepth = 0,
        .maxDepth = 1,
    };
}

VkRect2D PipelineManager::createScissor( const Rectf &scissor ) const noexcept
{
    return VkRect2D {
        .offset = {
            static_cast< crimild::Int32 >( scissor.getX() ),
            static_cast< crimild::Int32 >( scissor.getY() ),
        },
        .extent = VkExtent2D {
            static_cast< crimild::UInt32 >( scissor.getWidth() ),
            static_cast< crimild::UInt32 >( scissor.getHeight() ),
        },
    };
}

VkPipelineViewportStateCreateInfo PipelineManager::createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) const noexcept
{
    return VkPipelineViewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };
}

VkPipelineRasterizationStateCreateInfo PipelineManager::createRasterizer( void ) const noexcept
{
    return VkPipelineRasterizationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE, // VK_TRUE might be required for shadow maps
        .rasterizerDiscardEnable = VK_FALSE, // VK_TRUE disables output to the framebuffer
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE, // Might be needed for shadow mapping
        .depthBiasConstantFactor = 0,
        .depthBiasClamp = 0,
        .depthBiasSlopeFactor = 0,
    };
}

VkPipelineMultisampleStateCreateInfo PipelineManager::createMultiplesampleState( void ) const noexcept
{
    return VkPipelineMultisampleStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };
}

VkPipelineDepthStencilStateCreateInfo PipelineManager::createDepthStencilState( void ) const noexcept
{
    return VkPipelineDepthStencilStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
    };
}

VkPipelineColorBlendAttachmentState PipelineManager::createColorBlendAttachment( void ) const noexcept
{
    return VkPipelineColorBlendAttachmentState {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
    };
}

VkPipelineColorBlendStateCreateInfo PipelineManager::createColorBlending( const VkPipelineColorBlendAttachmentState &colorBlendAttachment ) const noexcept
{
    return VkPipelineColorBlendStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants[ 0 ] = 0.0f,
        .blendConstants[ 1 ] = 0.0f,
        .blendConstants[ 2 ] = 0.0f,
        .blendConstants[ 3 ] = 0.0f,
    };
}

