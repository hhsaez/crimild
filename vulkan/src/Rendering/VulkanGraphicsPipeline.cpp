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

#include "Rendering/VulkanGraphicsPipeline.hpp"

#include "Foundation/Log.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

namespace crimild {

    namespace vulkan {

        class ShaderModule {
        public:
            ShaderModule( RenderDevice *renderDevice, SharedPointer< Shader > const &shader ) noexcept
                : m_renderDevice( renderDevice )
            {
                CRIMILD_LOG_DEBUG( "Creating shader module for stage ", shader->getStageDescription() );

                assert( shader != nullptr && "Shader instance is null" );

                auto code = shader->getData();
                if ( shader->getDataType() == Shader::DataType::INLINE ) {
                    auto source = std::string( code.data(), code.size() );
                    if ( !renderDevice->getShaderCompiler().compile( shader->getStage(), source, code ) ) {
                        CRIMILD_LOG_FATAL( "Failed to create shader module" );
                        exit( EXIT_FAILURE );
                    }
                }

                auto createInfo = VkShaderModuleCreateInfo {
                    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                    .codeSize = code.size(),
                    .pCode = reinterpret_cast< const uint32_t * >( code.data() ),
                };

                if ( vkCreateShaderModule( renderDevice->getHandle(), &createInfo, nullptr, &m_handle ) != VK_SUCCESS ) {
                    CRIMILD_LOG_FATAL( "Failed to create shader module" );
                    exit( EXIT_FAILURE );
                }

                m_stage = utils::getVulkanShaderStageFlag( shader->getStage() );
                m_entryPointName = shader->getEntryPointName();
            }

            ~ShaderModule( void ) noexcept
            {
                vkDestroyShaderModule( m_renderDevice->getHandle(), m_handle, nullptr );
                m_handle = VK_NULL_HANDLE;
            }

            inline VkShaderModule getHandle( void ) const noexcept { return m_handle; }
            inline VkShaderStageFlagBits getStage( void ) const noexcept { return m_stage; }
            inline const char *getEntryPointName( void ) const noexcept { return m_entryPointName.c_str(); }

        private:
            RenderDevice *m_renderDevice = nullptr;
            VkShaderModule m_handle = VK_NULL_HANDLE;
            VkShaderStageFlagBits m_stage;
            std::string m_entryPointName;
        };

        static std::vector< std::unique_ptr< ShaderModule > > createShaderModules( RenderDevice *renderDevice, const ShaderProgram *program ) noexcept
        {
            CRIMILD_LOG_TRACE();

            std::vector< std::unique_ptr< ShaderModule > > modules;
            assert( program != nullptr && "Invalid shader program instance" );
            assert( !program->getShaders().empty() && "Invalid shader program" );
            program->getShaders().each( [ &modules, renderDevice ]( SharedPointer< Shader > &shader ) {
                modules.push_back( std::make_unique< ShaderModule >( renderDevice, shader ) );
            } );

            return modules;
        }

        static std::vector< VkPipelineShaderStageCreateInfo > createShaderStages( const std::vector< std::unique_ptr< ShaderModule > > &modules ) noexcept
        {
            CRIMILD_LOG_TRACE();

            std::vector< VkPipelineShaderStageCreateInfo > shaderStages;
            for ( const auto &module : modules ) {
                auto stage = VkPipelineShaderStageCreateInfo {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .stage = module->getStage(),
                    .module = module->getHandle(),
                    .pName = module->getEntryPointName(),
                };
                shaderStages.push_back( stage );
            }
            return shaderStages;
        }

        static std::vector< VkVertexInputBindingDescription > getVertexInputBindingDescriptions( const std::vector< VertexLayout > &vertexLayouts ) noexcept
        {
            std::vector< VkVertexInputBindingDescription > ret;
            for ( const auto &vertexLayout : vertexLayouts ) {
                ret.push_back(
                    VkVertexInputBindingDescription {
                        .binding = uint32_t( ret.size() ),
                        .stride = vertexLayout.getSize(),
                        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                    } );
            }
            // graphicsPipeline->getProgram()->instanceLayouts.each(
            //     [ & ]( const auto &layout ) {
            //         ret.add( VkVertexInputBindingDescription {
            //             .binding = uint32_t( ret.size() ),
            //             .stride = layout.getSize(),
            //             .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
            //         } );
            //     } );
            return ret;
        }

        static std::vector< VkVertexInputAttributeDescription > getVertexInputAttributeDescriptions( RenderDevice *renderDevice, const std::vector< VertexLayout > &vertexLayouts ) noexcept
        {
            std::vector< VkVertexInputAttributeDescription > attributeDescriptions;

            Index binding = 0;
            Index location = 0;

            for ( const auto &vertexLayout : vertexLayouts ) {
                vertexLayout.eachAttribute(
                    [ & ]( const auto &attrib ) {
                        attributeDescriptions.push_back(
                            VkVertexInputAttributeDescription {
                                .location = crimild::UInt32( location++ ),
                                .binding = crimild::UInt32( binding ),
                                .format = utils::getFormat( attrib.format ),
                                .offset = attrib.offset,
                            } );
                    } );
                binding++;
            }

            // graphicsPipeline->getProgram()->instanceLayouts.each(
            //     [ & ]( const auto &layout ) {
            //         layout.eachAttribute(
            //             [ & ]( const auto &attrib ) {
            //                 attributeDescriptions.add(
            //                     VkVertexInputAttributeDescription {
            //                         .location = crimild::UInt32( location++ ),
            //                         .binding = crimild::UInt32( binding ),
            //                         .format = utils::getFormat( renderDevice, attrib.format ),
            //                         .offset = attrib.offset,
            //                     } );
            //             } );
            //         binding++;
            //     } );
            return attributeDescriptions;
        }

        static VkPipelineVertexInputStateCreateInfo createVertexInput( const std::vector< VkVertexInputBindingDescription > &bindingDescriptions, const std::vector< VkVertexInputAttributeDescription > &attributeDescriptions ) noexcept
        {
            return VkPipelineVertexInputStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = static_cast< crimild::UInt32 >( bindingDescriptions.size() ),
                .pVertexBindingDescriptions = bindingDescriptions.data(),
                .vertexAttributeDescriptionCount = static_cast< crimild::UInt32 >( attributeDescriptions.size() ),
                .pVertexAttributeDescriptions = attributeDescriptions.data(),
            };
        }

        static VkPipelineInputAssemblyStateCreateInfo createInputAssemby( Primitive::Type primitiveType ) noexcept
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

        VkViewport createViewport( RenderDevice *renderDevice, const ViewportDimensions &viewport ) noexcept
        {
            auto x = viewport.dimensions.origin.x;
            auto y = viewport.dimensions.origin.y;
            auto w = viewport.dimensions.size.width;
            auto h = viewport.dimensions.size.height;
            auto minD = viewport.depthRange.x;
            auto maxD = viewport.depthRange.y;

            if ( viewport.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
                const auto extent = renderDevice->getSwapchainExtent();
                x *= extent.width;
                y *= extent.height;
                w *= extent.width;
                h *= extent.height;
            }

            // Because Vulkan's coordinate system is different from Crimild's one,
            // we need to specify the viewport in a different way than usual.
            // WARNING: This trick requires VK_KHR_maintenance1 support (which should
            // be part of the core spec at the time of this writing).
            // See: https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
            //
            // Hernan: I tried the trick specified here:
            // https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
            // but didn't really worked for me. On one hand, things like computing
            // reflection or refraction required me to reverse the resulting vector
            // which is very error prone. On the other, the view is zoomed with respect
            // to other platforms like OpenGL (this might be a bug, though).
            // Also, don't forget to reverse face culling (see createRasterizer below)

            return VkViewport {
                .x = x,
                .y = h + y,
                .width = w,
                .height = -h,
                .minDepth = minD,
                .maxDepth = maxD,
            };
        }

        VkRect2D createScissor( RenderDevice *renderDevice, const ViewportDimensions &scissor ) noexcept
        {
            auto x = scissor.dimensions.origin.x;
            auto y = scissor.dimensions.origin.y;
            auto w = scissor.dimensions.size.width;
            auto h = scissor.dimensions.size.height;

            if ( scissor.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
                const auto extent = renderDevice->getSwapchainExtent();
                x *= extent.width;
                y *= extent.height;
                w *= extent.width;
                h *= extent.height;
            }

            return VkRect2D {
                .offset = {
                    static_cast< crimild::Int32 >( x ),
                    static_cast< crimild::Int32 >( y ),
                },
                .extent = VkExtent2D {
                    static_cast< crimild::UInt32 >( w ),
                    static_cast< crimild::UInt32 >( h ),
                },
            };
        }

        VkPipelineViewportStateCreateInfo createViewportState( const VkViewport &viewport, const VkRect2D &scissor ) noexcept
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

        VkPipelineViewportStateCreateInfo createDynamicViewportState( crimild::Bool hasViewport, crimild::Bool hasScissor ) noexcept
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

        VkPipelineRasterizationStateCreateInfo createRasterizer( const RasterizationState &rasterizationState ) noexcept
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

        VkPipelineMultisampleStateCreateInfo createMultiplesampleState( void ) noexcept
        {
            // auto physicalDevice = renderDevice->physicalDevice;
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

        VkPipelineDepthStencilStateCreateInfo createDepthStencilState( const DepthStencilState &state ) noexcept
        {
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

        VkPipelineColorBlendAttachmentState createColorBlendAttachment( const ColorBlendState &state ) noexcept
        {
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

        VkPipelineColorBlendStateCreateInfo createColorBlending( const std::vector< VkPipelineColorBlendAttachmentState > &colorBlendAttachments ) noexcept
        {
            return VkPipelineColorBlendStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .logicOpEnable = VK_FALSE,
                .logicOp = VK_LOGIC_OP_COPY,
                .attachmentCount = UInt32( colorBlendAttachments.size() ),
                .pAttachments = colorBlendAttachments.data(),
                .blendConstants = { 0, 0, 0, 0 },
            };
        }

        std::vector< VkDynamicState > getDynamicStates( const ViewportDimensions &viewport, const ViewportDimensions &scissor ) noexcept
        {
            std::vector< VkDynamicState > dynamicStates;

            if ( viewport.scalingMode == ScalingMode::DYNAMIC ) {
                dynamicStates.push_back( VK_DYNAMIC_STATE_VIEWPORT );
            }

            if ( scissor.scalingMode == ScalingMode::DYNAMIC ) {
                dynamicStates.push_back( VK_DYNAMIC_STATE_SCISSOR );
            }

            return dynamicStates;
        }

        VkPipelineDynamicStateCreateInfo createDynamicState( std::vector< VkDynamicState > &dynamicStates ) noexcept
        {
            return VkPipelineDynamicStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .flags = 0,
                .dynamicStateCount = static_cast< crimild::UInt32 >( dynamicStates.size() ),
                .pDynamicStates = dynamicStates.data(),
            };
        }

        VkPipelineLayout createPipelineLayout( RenderDevice *renderDevice, const std::vector< VkDescriptorSetLayout > &descriptorSetLayouts ) noexcept
        {
            auto createInfo = VkPipelineLayoutCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .setLayoutCount = static_cast< crimild::UInt32 >( descriptorSetLayouts.size() ),
                .pSetLayouts = descriptorSetLayouts.data(),
                .pushConstantRangeCount = 0,
                .pPushConstantRanges = nullptr,
            };

            VkPipelineLayout pipelineLayout;
            CRIMILD_VULKAN_CHECK(
                vkCreatePipelineLayout(
                    renderDevice->getHandle(),
                    &createInfo,
                    nullptr,
                    &pipelineLayout ) );

            return pipelineLayout;
        }

    }

}

vulkan::GraphicsPipeline::GraphicsPipeline(
    RenderDevice *renderDevice,
    VkRenderPass renderPass,
    const std::vector< VkDescriptorSetLayout > &descriptorSetLayouts,
    const ShaderProgram *program,
    const std::vector< VertexLayout > &vertexLayouts,
    const DepthStencilState &pipelineDepthStencilState,
    const RasterizationState &pipelineRasterizationState,
    const ColorBlendState &pipelineColorBlendState ) noexcept
    : m_renderDevice( renderDevice->getHandle() )
{
    CRIMILD_LOG_TRACE();

    const auto pipelineViewport = ViewportDimensions {};
    const auto pipelineScissor = ViewportDimensions {};

    // WARNING: all of these config params are used when creating the graphicsPipeline and
    // they must be alive when vkCreatePipeline is called. Beware of scopes!
    auto shaderModules = createShaderModules( renderDevice, program );
    auto shaderStages = createShaderStages( shaderModules );
    auto vertexBindingDescriptions = getVertexInputBindingDescriptions( vertexLayouts );
    auto vertexAttributeDescriptions = getVertexInputAttributeDescriptions( renderDevice, vertexLayouts );
    auto vertexInputInfo = createVertexInput( vertexBindingDescriptions, vertexAttributeDescriptions );
    auto inputAssembly = createInputAssemby( Primitive::Type::TRIANGLES );
    auto viewport = createViewport( renderDevice, pipelineViewport );
    auto scissor = createScissor( renderDevice, pipelineScissor );
    auto viewportState = pipelineViewport.scalingMode != ScalingMode::DYNAMIC
                             ? createViewportState( viewport, scissor )
                             : createDynamicViewportState( true, true );
    auto rasterizer = createRasterizer( pipelineRasterizationState );
    auto multisampleState = createMultiplesampleState();
    auto depthStencilState = createDepthStencilState( pipelineDepthStencilState );

    std::vector< VkPipelineColorBlendAttachmentState > colorBlendAttachments = {
        // TODO: We must create one blend state for each attachment
        createColorBlendAttachment( pipelineColorBlendState ),
    };
    auto colorBlending = createColorBlending( colorBlendAttachments );

    auto dynamicStates = getDynamicStates( pipelineViewport, pipelineScissor );
    auto dynamicState = createDynamicState( dynamicStates );

    // Create pipeline layout
    m_pipelineLayout = createPipelineLayout( renderDevice, descriptorSetLayouts );

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
        .layout = m_pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE, // Optional
        .basePipelineIndex = -1,              // Optional
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateGraphicsPipelines(
            renderDevice->getHandle(),
            VK_NULL_HANDLE,
            1,
            &createInfo,
            nullptr,
            &m_pipeline ) );

#if 0
    auto renderPass = renderDevice->getBindInfo( renderDevice->getCurrentRenderPass() );

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
#endif
}

vulkan::GraphicsPipeline::~GraphicsPipeline( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyPipeline(
        m_renderDevice,
        m_pipeline,
        nullptr );

    vkDestroyPipelineLayout(
        m_renderDevice,
        m_pipelineLayout,
        nullptr );

    m_pipeline = VK_NULL_HANDLE;
    m_pipelineLayout = VK_NULL_HANDLE;
    m_renderDevice = VK_NULL_HANDLE;
}
