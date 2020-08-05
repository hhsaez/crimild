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
#include "VulkanPhysicalDevice.hpp"
#include "Rendering/CullFaceState.hpp"
#include "Rendering/DepthState.hpp"
#include "Rendering/PolygonState.hpp"
#include "Rendering/RenderPass.hpp"
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
    auto vertexAttributeDescriptions = getVertexInputAttributeDescriptions( renderDevice, pipeline );
    auto vertexInputInfo = createVertexInput( vertexBindingDescriptions, vertexAttributeDescriptions );
    auto inputAssembly = createInputAssemby( pipeline->primitiveType );
    auto viewport = createViewport( pipeline->viewport );
    auto scissor = createScissor( pipeline->scissor );
    auto viewportState = pipeline->viewport.scalingMode != ScalingMode::DYNAMIC
    	? createViewportState( viewport, scissor )
    	: createDynamicViewportState( true, true );
    auto rasterizer = createRasterizer( pipeline );
    auto multisampleState = createMultiplesampleState();
    auto depthStencilState = createDepthStencilState( pipeline );
    auto colorBlendAttachment = createColorBlendAttachment();
    auto colorBlending = createColorBlending( colorBlendAttachment );
    auto dynamicStates = getDynamicStates( pipeline );
    auto dynamicState = createDynamicState( dynamicStates );

    auto pipelineLayout = renderDevice->create(
        PipelineLayout::Descriptor {
            .setLayouts = pipeline->program->descriptorSetLayouts.map(
            	[]( auto &layout ) {
                	return crimild::get_ptr( layout );
            	}
            ),
        }
    );

    auto renderPass = renderDevice->getBindInfo( renderDevice->getCurrentRenderPass() );

    auto createInfo = VkGraphicsPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast< uint32_t >( shaderStages.size() ),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = &depthStencilState,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = pipelineLayout->handler,
        .renderPass = renderPass.handler,
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

    setBindInfo(
        pipeline,
           {
            .pipelineHandler = pipelineHander,
            .pipelineLayout = pipelineLayout,
        }
    );

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

    auto bindInfo = getBindInfo( pipeline );

    vkDestroyPipeline(
        renderDevice->handler,
        bindInfo.pipelineHandler,
        nullptr
    );

    removeBindInfo( pipeline );

    return ManagerImpl::unbind( pipeline );
}

PipelineManager::ShaderModuleArray PipelineManager::createShaderModules( RenderDevice *renderDevice, ShaderProgram *program ) const noexcept
{
    CRIMILD_LOG_TRACE( "Creating shader modules" );

    ShaderModuleArray modules;
    assert( program != nullptr && "Invalid shader program instance" );
    assert( !program->getShaders().empty() && "Invalid shader program" );
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

Array< VkVertexInputBindingDescription > PipelineManager::getVertexInputBindingDescriptions( Pipeline *pipeline ) const noexcept
{
    return pipeline->program->vertexLayouts.map(
        [&, binding = 0]( const auto &layout ) mutable {
            return VkVertexInputBindingDescription {
                .binding = uint32_t( binding++ ),
                .stride = layout.getSize(),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            };
        }
    );
}

Array< VkVertexInputAttributeDescription > PipelineManager::getVertexInputAttributeDescriptions( RenderDevice *renderDevice, Pipeline *pipeline ) const noexcept
{
    Array< VkVertexInputAttributeDescription > attributeDescriptions;
    pipeline->program->vertexLayouts.each(
        [&, binding = 0 ]( const auto &layout ) mutable {
            layout.eachAttribute(
                [&, location = 0]( const auto &attrib ) mutable {
                    attributeDescriptions.add(
                        VkVertexInputAttributeDescription {
                            .binding = crimild::UInt32( binding ),
                            .location = crimild::UInt32( location++ ),
                            .format = utils::getFormat( renderDevice, attrib.format ),
                            .offset = attrib.offset,
                        }
                    );
                }
            );
            binding++;
        }
    );
    return attributeDescriptions;
}

VkPipelineVertexInputStateCreateInfo PipelineManager::createVertexInput( const Array< VkVertexInputBindingDescription > &bindingDescriptions, const Array< VkVertexInputAttributeDescription > &attributeDescriptions ) const noexcept
{
    return VkPipelineVertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = static_cast< crimild::UInt32 >( bindingDescriptions.size() ),
        .pVertexBindingDescriptions = bindingDescriptions.getData(),
        .vertexAttributeDescriptionCount = static_cast< crimild::UInt32 >( attributeDescriptions.size() ),
        .pVertexAttributeDescriptions = attributeDescriptions.getData(),
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

VkViewport PipelineManager::createViewport( const ViewportDimensions &viewport ) const noexcept
{
    return utils::getViewport( &viewport, getRenderDevice() );
}

VkRect2D PipelineManager::createScissor( const ViewportDimensions &scissor ) const noexcept
{
    return utils::getScissor( &scissor, getRenderDevice() );
}

VkPipelineViewportStateCreateInfo PipelineManager::createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) const noexcept
{
    return VkPipelineViewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
        .flags = 0,
    };
}

VkPipelineViewportStateCreateInfo PipelineManager::createDynamicViewportState( crimild::Bool hasViewport, crimild::Bool hasScissor ) const noexcept
{
    return VkPipelineViewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = hasViewport ? 1u : 0,
        .pViewports = nullptr,
        .scissorCount = hasScissor ? 1u : 0,
        .pScissors = nullptr,
        .flags = 0,
    };
}

VkPipelineRasterizationStateCreateInfo PipelineManager::createRasterizer( Pipeline *pipeline ) const noexcept
{
    auto polygonMode = VK_POLYGON_MODE_FILL;
    auto lineWidth = 1.0f;
    if ( auto polygonState = crimild::get_ptr( pipeline->polygonState ) ) {
        polygonMode = utils::getPolygonMode( polygonState );
        lineWidth = polygonState->lineWidth;
    }

    auto cullMode = VK_CULL_MODE_BACK_BIT;
    auto frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    if ( auto cullFaceState = crimild::get_ptr( pipeline->cullFaceState ) ) {
        if ( !cullFaceState->isEnabled() ) {
            cullMode = VK_CULL_MODE_NONE;
        }
    }

    auto &rasterizationState = pipeline->rasterizationState;

    return VkPipelineRasterizationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = rasterizationState.depthClampEnable,
        .rasterizerDiscardEnable = rasterizationState.rasterizerDiscardEnable,
        .polygonMode = polygonMode,
        .lineWidth = lineWidth,
        .cullMode = cullMode,
        .frontFace = frontFace,
        .depthBiasEnable = rasterizationState.depthBiasEnable,
        .depthBiasConstantFactor = rasterizationState.depthBiasConstantFactor,
        .depthBiasClamp = rasterizationState.depthBiasClamp,
        .depthBiasSlopeFactor = rasterizationState.depthBiasSlopeFactor,
    };
}

VkPipelineMultisampleStateCreateInfo PipelineManager::createMultiplesampleState( void ) const noexcept
{
    auto renderDevice = getRenderDevice();
    auto physicalDevice = renderDevice->physicalDevice;
    auto msaaSamples = VK_SAMPLE_COUNT_1_BIT; //physicalDevice->msaaSamples;

    return VkPipelineMultisampleStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = msaaSamples,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };
}

VkPipelineDepthStencilStateCreateInfo PipelineManager::createDepthStencilState( Pipeline *pipeline ) const noexcept
{
    auto &state = pipeline->depthStencilState;

    static auto getStencilOp = []( auto in ) {
        switch ( in ) {
            case StencilOp::KEEP:
                return VK_STENCIL_OP_KEEP;
            case StencilOp::ZERO:
                return VK_STENCIL_OP_ZERO;
            case StencilOp::REPLACE:
                return VK_STENCIL_OP_REPLACE;
            case StencilOp::INCREMENT_AND_CLAMP:
                return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            case StencilOp::DECREMENT_AND_CLAMP:
                return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            case StencilOp::INVERT:
                return VK_STENCIL_OP_INVERT;
            case StencilOp::INCREMENT_AND_WRAP:
                return VK_STENCIL_OP_INCREMENT_AND_WRAP;
            case StencilOp::DECREMENT_AND_WRAP:
                return VK_STENCIL_OP_DECREMENT_AND_WRAP;
            default:
                CRIMILD_LOG_ERROR( "Invalid StencilOp value: ", Int32( in ) );
                return VK_STENCIL_OP_KEEP;
        }
    };

    static auto getStencilOpState = []( auto &in ) {
        return VkStencilOpState {
            .failOp = getStencilOp( in.failOp ),
            .passOp = getStencilOp( in.passOp ),
            .depthFailOp = getStencilOp( in.depthFailOp ),
            .compareOp = utils::getCompareOp( in.compareOp ),
            .compareMask = in.compareMask,
            .writeMask = in.writeMask,
            .reference = in.reference,
        };
    };

    return VkPipelineDepthStencilStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = state.depthTestEnable,
        .depthWriteEnable = state.depthWriteEnable,
        .depthCompareOp = utils::getCompareOp( state.depthCompareOp ),
        .depthBoundsTestEnable = state.depthBoundsTestEnable,
        .minDepthBounds = state.minDepthBounds,
        .maxDepthBounds = state.maxDepthBounds,
        .stencilTestEnable = state.stencilTestEnable,
        .front = getStencilOpState( state.front ),
        .back = getStencilOpState( state.back ),
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

PipelineManager::DynamicStates PipelineManager::getDynamicStates( Pipeline *pipeline ) const noexcept
{
    std::vector< VkDynamicState > dynamicStates;

    if ( pipeline->viewport.scalingMode == ScalingMode::DYNAMIC ) {
        dynamicStates.push_back( VK_DYNAMIC_STATE_VIEWPORT );
    }

    if ( pipeline->scissor.scalingMode == ScalingMode::DYNAMIC ) {
        dynamicStates.push_back( VK_DYNAMIC_STATE_SCISSOR );
    }

    return dynamicStates;
}

VkPipelineDynamicStateCreateInfo PipelineManager::createDynamicState( DynamicStates &dynamicStates ) const noexcept
{
    return VkPipelineDynamicStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pDynamicStates = dynamicStates.data(),
        .dynamicStateCount = static_cast< crimild::UInt32 >( dynamicStates.size() ),
        .flags = 0,
    };
}
