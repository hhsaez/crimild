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

#include "VulkanGraphicsPipelineOLD.hpp"

#include "Foundation/Log.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool GraphicsPipelineManager::bind( crimild::GraphicsPipeline *graphicsPipeline ) noexcept
{
    if ( validate( graphicsPipeline ) ) {
        // GraphicsPipeline already bound
        return true;
    }

    CRIMILD_LOG_TRACE();

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        return false;
    }

    // WARNING: all of these config params are used when creating the graphicsPipeline and
    // they must be alive when vkCreatePipeline is called. Beware of scopes!
    auto shaderModules = createShaderModules( renderDevice, graphicsPipeline->getProgram() );
    auto shaderStages = createShaderStages( shaderModules );
    auto vertexBindingDescriptions = getVertexInputBindingDescriptions( graphicsPipeline );
    auto vertexAttributeDescriptions = getVertexInputAttributeDescriptions( renderDevice, graphicsPipeline );
    auto vertexInputInfo = createVertexInput( vertexBindingDescriptions, vertexAttributeDescriptions );
    auto inputAssembly = createInputAssemby( graphicsPipeline->primitiveType );
    auto viewport = createViewport( graphicsPipeline->viewport );
    auto scissor = createScissor( graphicsPipeline->scissor );
    auto viewportState = graphicsPipeline->viewport.scalingMode != ScalingMode::DYNAMIC
                             ? createViewportState( viewport, scissor )
                             : createDynamicViewportState( true, true );
    auto rasterizer = createRasterizer( graphicsPipeline );
    auto multisampleState = createMultiplesampleState();
    auto depthStencilState = createDepthStencilState( graphicsPipeline );
    auto colorBlendAttachments = renderDevice->getCurrentRenderPass()
                                     ->attachments
                                     .filter(
                                         []( auto att ) {
                                             return !utils::formatIsDepthStencil( att->format );
                                         } )
                                     .map(
                                         [ & ]( auto att ) {
                                             return createColorBlendAttachment( graphicsPipeline );
                                         } );
    // auto colorBlendAttachment = createColorBlendAttachment( graphicsPipeline );
    auto colorBlending = createColorBlending( colorBlendAttachments );
    auto dynamicStates = getDynamicStates( graphicsPipeline );
    auto dynamicState = createDynamicState( dynamicStates );

    auto pipelineLayout = renderDevice->create(
        PipelineLayout::Descriptor {
            .setLayouts = graphicsPipeline->getProgram()->descriptorSetLayouts.map(
                []( auto &layout ) {
                    return crimild::get_ptr( layout );
                } ),
        } );

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
        .basePipelineIndex = -1,              // Optional
    };

    VkPipeline pipelineHander;
    CRIMILD_VULKAN_CHECK(
        vkCreateGraphicsPipelines(
            renderDevice->handler,
            VK_NULL_HANDLE,
            1,
            &createInfo,
            nullptr,
            &pipelineHander ) );

    setBindInfo(
        graphicsPipeline,
        {
            .pipelineHandler = pipelineHander,
            .pipelineLayout = pipelineLayout,
        } );

    utils::setObjectName(
        renderDevice->handler,
        UInt64( pipelineHander ),
        VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT,
        graphicsPipeline->getName().c_str() );

    return ManagerImpl::bind( graphicsPipeline );
}

crimild::Bool GraphicsPipelineManager::unbind( crimild::GraphicsPipeline *graphicsPipeline ) noexcept
{
    if ( !validate( graphicsPipeline ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE();

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto bindInfo = getBindInfo( graphicsPipeline );

    vkDestroyPipeline(
        renderDevice->handler,
        bindInfo.pipelineHandler,
        nullptr );

    removeBindInfo( graphicsPipeline );

    return ManagerImpl::unbind( graphicsPipeline );
}

GraphicsPipelineManager::ShaderModuleArray GraphicsPipelineManager::createShaderModules( RenderDeviceOLD *renderDevice, ShaderProgram *program ) const noexcept
{
    CRIMILD_LOG_TRACE();

    ShaderModuleArray modules;
    assert( program != nullptr && "Invalid shader program instance" );
    assert( !program->getShaders().empty() && "Invalid shader program" );
    program->getShaders().each( [ &modules, renderDevice ]( SharedPointer< Shader > &shader ) {
        auto module = renderDevice->create(
            ShaderModule::Descriptor {
                .shader = crimild::get_ptr( shader ) } );
        if ( module != nullptr ) {
            modules.push_back( module );
        }
    } );

    return modules;
}

GraphicsPipelineManager::ShaderStageArray GraphicsPipelineManager::createShaderStages( const ShaderModuleArray &modules ) const noexcept
{
    CRIMILD_LOG_TRACE();

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

Array< VkVertexInputBindingDescription > GraphicsPipelineManager::getVertexInputBindingDescriptions( crimild::GraphicsPipeline *graphicsPipeline ) const noexcept
{
    Array< VkVertexInputBindingDescription > ret;
    graphicsPipeline->getProgram()->vertexLayouts.each(
        [ & ]( const auto &layout ) {
            ret.add( VkVertexInputBindingDescription {
                .binding = uint32_t( ret.size() ),
                .stride = layout.getSize(),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            } );
        } );
    graphicsPipeline->getProgram()->instanceLayouts.each(
        [ & ]( const auto &layout ) {
            ret.add( VkVertexInputBindingDescription {
                .binding = uint32_t( ret.size() ),
                .stride = layout.getSize(),
                .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
            } );
        } );
    return ret;
}

Array< VkVertexInputAttributeDescription > GraphicsPipelineManager::getVertexInputAttributeDescriptions( RenderDeviceOLD *renderDevice, crimild::GraphicsPipeline *graphicsPipeline ) const noexcept
{
    Array< VkVertexInputAttributeDescription > attributeDescriptions;

    Index binding = 0;
    Index location = 0;

    graphicsPipeline->getProgram()->vertexLayouts.each(
        [ & ]( const auto &layout ) {
            layout.eachAttribute(
                [ & ]( const auto &attrib ) {
                    attributeDescriptions.add(
                        VkVertexInputAttributeDescription {
                            .location = crimild::UInt32( location++ ),
                            .binding = crimild::UInt32( binding ),
                            .format = utils::getFormat( renderDevice, attrib.format ),
                            .offset = attrib.offset,
                        } );
                } );
            binding++;
        } );
    graphicsPipeline->getProgram()->instanceLayouts.each(
        [ & ]( const auto &layout ) {
            layout.eachAttribute(
                [ & ]( const auto &attrib ) {
                    attributeDescriptions.add(
                        VkVertexInputAttributeDescription {
                            .location = crimild::UInt32( location++ ),
                            .binding = crimild::UInt32( binding ),
                            .format = utils::getFormat( renderDevice, attrib.format ),
                            .offset = attrib.offset,
                        } );
                } );
            binding++;
        } );
    return attributeDescriptions;
}

VkPipelineVertexInputStateCreateInfo GraphicsPipelineManager::createVertexInput( const Array< VkVertexInputBindingDescription > &bindingDescriptions, const Array< VkVertexInputAttributeDescription > &attributeDescriptions ) const noexcept
{
    return VkPipelineVertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = static_cast< crimild::UInt32 >( bindingDescriptions.size() ),
        .pVertexBindingDescriptions = bindingDescriptions.getData(),
        .vertexAttributeDescriptionCount = static_cast< crimild::UInt32 >( attributeDescriptions.size() ),
        .pVertexAttributeDescriptions = attributeDescriptions.getData(),
    };
}

VkPipelineInputAssemblyStateCreateInfo GraphicsPipelineManager::createInputAssemby( Primitive::Type primitiveType ) const noexcept
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

VkViewport GraphicsPipelineManager::createViewport( const ViewportDimensions &viewport ) const noexcept
{
    return utils::getViewport( &viewport, getRenderDevice() );
}

VkRect2D GraphicsPipelineManager::createScissor( const ViewportDimensions &scissor ) const noexcept
{
    return utils::getScissor( &scissor, getRenderDevice() );
}

VkPipelineViewportStateCreateInfo GraphicsPipelineManager::createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) const noexcept
{
    return VkPipelineViewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };
}

VkPipelineViewportStateCreateInfo GraphicsPipelineManager::createDynamicViewportState( crimild::Bool hasViewport, crimild::Bool hasScissor ) const noexcept
{
    return VkPipelineViewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .flags = 0,
        .viewportCount = hasViewport ? 1u : 0,
        .pViewports = nullptr,
        .scissorCount = hasScissor ? 1u : 0,
        .pScissors = nullptr,
    };
}

VkPipelineRasterizationStateCreateInfo GraphicsPipelineManager::createRasterizer( crimild::GraphicsPipeline *graphicsPipeline ) const noexcept
{
    static auto getVkPolygonMode = []( auto input ) {
        switch ( input ) {
            case PolygonMode::FILL:
                return VK_POLYGON_MODE_FILL;
            case PolygonMode::LINE:
                return VK_POLYGON_MODE_LINE;
            case PolygonMode::POINT:
                return VK_POLYGON_MODE_POINT;
            default:
                return VK_POLYGON_MODE_FILL;
        }
    };

    static auto getVkFrontFace = []( auto input ) {
        switch ( input ) {
            case FrontFace::COUNTER_CLOCKWISE:
                return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            case FrontFace::CLOCKWISE:
                return VK_FRONT_FACE_CLOCKWISE;
            default:
                return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
    };

    static auto getVkCullModeFlag = []( auto input ) {
        switch ( input ) {
            case CullMode::NONE:
                return VK_CULL_MODE_NONE;
            case CullMode::FRONT:
                return VK_CULL_MODE_FRONT_BIT;
            case CullMode::BACK:
                return VK_CULL_MODE_BACK_BIT;
            case CullMode::FRONT_AND_BACK:
                return VK_CULL_MODE_FRONT_AND_BACK;
            default:
                return VK_CULL_MODE_FRONT_BIT;
        }
    };

    auto &rasterizationState = graphicsPipeline->rasterizationState;

    return VkPipelineRasterizationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = rasterizationState.depthClampEnable,
        .rasterizerDiscardEnable = rasterizationState.rasterizerDiscardEnable,
        .polygonMode = getVkPolygonMode( rasterizationState.polygonMode ),
        .cullMode = static_cast< VkCullModeFlags >( getVkCullModeFlag( rasterizationState.cullMode ) ),
        .frontFace = getVkFrontFace( rasterizationState.frontFace ),
        .depthBiasEnable = rasterizationState.depthBiasEnable,
        .depthBiasConstantFactor = rasterizationState.depthBiasConstantFactor,
        .depthBiasClamp = rasterizationState.depthBiasClamp,
        .depthBiasSlopeFactor = rasterizationState.depthBiasSlopeFactor,
        .lineWidth = rasterizationState.lineWidth,
    };
}

VkPipelineMultisampleStateCreateInfo GraphicsPipelineManager::createMultiplesampleState( void ) const noexcept
{
    auto renderDevice = getRenderDevice();
    auto physicalDevice = renderDevice->physicalDevice;
    auto msaaSamples = VK_SAMPLE_COUNT_1_BIT; //physicalDevice->msaaSamples;

    return VkPipelineMultisampleStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = msaaSamples,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };
}

VkPipelineDepthStencilStateCreateInfo GraphicsPipelineManager::createDepthStencilState( crimild::GraphicsPipeline *graphicsPipeline ) const noexcept
{
    auto &state = graphicsPipeline->depthStencilState;

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
        .stencilTestEnable = state.stencilTestEnable,
        .front = getStencilOpState( state.front ),
        .back = getStencilOpState( state.back ),
        .minDepthBounds = state.minDepthBounds,
        .maxDepthBounds = state.maxDepthBounds,
    };
}

VkPipelineColorBlendAttachmentState GraphicsPipelineManager::createColorBlendAttachment( crimild::GraphicsPipeline *graphicsPipeline ) const noexcept
{
    auto &state = graphicsPipeline->colorBlendState;

    static auto getVkBlendFactor = []( auto input ) {
        switch ( input ) {
            case BlendFactor::ZERO:
                return VK_BLEND_FACTOR_ZERO;
            case BlendFactor::ONE:
                return VK_BLEND_FACTOR_ONE;
            case BlendFactor::SRC_COLOR:
                return VK_BLEND_FACTOR_SRC_COLOR;
            case BlendFactor::ONE_MINUS_SRC_COLOR:
                return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            case BlendFactor::DST_COLOR:
                return VK_BLEND_FACTOR_DST_COLOR;
            case BlendFactor::ONE_MINUS_DST_COLOR:
                return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            case BlendFactor::SRC_ALPHA:
                return VK_BLEND_FACTOR_SRC_ALPHA;
            case BlendFactor::ONE_MINUS_SRC_ALPHA:
                return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            case BlendFactor::DST_ALPHA:
                return VK_BLEND_FACTOR_DST_ALPHA;
            case BlendFactor::ONE_MINUS_DST_ALPHA:
                return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            case BlendFactor::CONSTANT_COLOR:
                return VK_BLEND_FACTOR_CONSTANT_COLOR;
            case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
                return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
            case BlendFactor::CONSTANT_ALPHA:
                return VK_BLEND_FACTOR_CONSTANT_ALPHA;
            case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
                return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
            case BlendFactor::SRC_ALPHA_SATURATE:
                return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
            case BlendFactor::SRC1_COLOR:
                return VK_BLEND_FACTOR_SRC1_COLOR;
            case BlendFactor::ONE_MINUS_SRC1_COLOR:
                return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
            case BlendFactor::SRC1_ALPHA:
                return VK_BLEND_FACTOR_SRC1_ALPHA;
            case BlendFactor::ONE_MINUS_SRC1_ALPHA:
                return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
            default:
                return VK_BLEND_FACTOR_ZERO;
        }
    };

    static auto getVkBlendOp = []( auto input ) {
        switch ( input ) {
            case BlendOp::ADD:
                return VK_BLEND_OP_ADD;
            case BlendOp::SUBTRACT:
                return VK_BLEND_OP_SUBTRACT;
            case BlendOp::REVERSE_SUBTRACT:
                return VK_BLEND_OP_REVERSE_SUBTRACT;
            case BlendOp::MIN:
                return VK_BLEND_OP_MIN;
            case BlendOp::MAX:
                return VK_BLEND_OP_MAX;
            default:
                return VK_BLEND_OP_ADD;
        }
    };

    return VkPipelineColorBlendAttachmentState {
        // TODO: Support color write mask in ColorBlendState
        .blendEnable = state.enable,
        .srcColorBlendFactor = getVkBlendFactor( state.srcColorBlendFactor ),
        .dstColorBlendFactor = getVkBlendFactor( state.dstColorBlendFactor ),
        .colorBlendOp = getVkBlendOp( state.colorBlendOp ),
        .srcAlphaBlendFactor = getVkBlendFactor( state.srcAlphaBlendFactor ),
        .dstAlphaBlendFactor = getVkBlendFactor( state.dstAlphaBlendFactor ),
        .alphaBlendOp = getVkBlendOp( state.alphaBlendOp ),
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
}

VkPipelineColorBlendStateCreateInfo GraphicsPipelineManager::createColorBlending( const Array< VkPipelineColorBlendAttachmentState > &colorBlendAttachments ) const noexcept
{
    return VkPipelineColorBlendStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = UInt32( colorBlendAttachments.size() ),
        .pAttachments = colorBlendAttachments.getData(),
        .blendConstants = { 0, 0, 0, 0 },
    };
}

GraphicsPipelineManager::DynamicStates GraphicsPipelineManager::getDynamicStates( crimild::GraphicsPipeline *graphicsPipeline ) const noexcept
{
    std::vector< VkDynamicState > dynamicStates;

    if ( graphicsPipeline->viewport.scalingMode == ScalingMode::DYNAMIC ) {
        dynamicStates.push_back( VK_DYNAMIC_STATE_VIEWPORT );
    }

    if ( graphicsPipeline->scissor.scalingMode == ScalingMode::DYNAMIC ) {
        dynamicStates.push_back( VK_DYNAMIC_STATE_SCISSOR );
    }

    return dynamicStates;
}

VkPipelineDynamicStateCreateInfo GraphicsPipelineManager::createDynamicState( DynamicStates &dynamicStates ) const noexcept
{
    return VkPipelineDynamicStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .flags = 0,
        .dynamicStateCount = static_cast< crimild::UInt32 >( dynamicStates.size() ),
        .pDynamicStates = dynamicStates.data(),
    };
}
