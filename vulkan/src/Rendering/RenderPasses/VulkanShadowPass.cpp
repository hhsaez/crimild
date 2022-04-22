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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/RenderPasses/VulkanShadowPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/perspective.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/FetchLights.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

ShadowPass::ShadowPass( RenderDevice *renderDevice ) noexcept
    : RenderPass( renderDevice )
{
    init();
}

ShadowPass::~ShadowPass( void ) noexcept
{
    clear();
}

Event ShadowPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            clear();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void ShadowPass::render( void ) noexcept
{
    auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
        return;
    }

    RenderableSet renderables;
    scene->perform(
        ApplyToGeometries(
            [ & ]( Geometry *geometry ) {
                if ( geometry->getLayer() == Node::Layer::DEFAULT ) {
                    renderables.addGeometry( geometry );
                }
            } ) );

    FetchLights fetch;
    scene->perform( fetch );
    Light *light = nullptr;
    fetch.forEachLight(
        [ & ]( auto l ) {
            if ( light == nullptr && l->getType() == Light::Type::DIRECTIONAL && l->castShadows() ) {
                light = l;
            }
        } );

    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    const auto clearValues = std::array< VkClearValue, 1 > {
        VkClearValue {
            .depthStencil = {
                1,
                0,
            },
        },
    };

    auto renderPassInfo = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[ currentFrameIndex ],
        .renderArea = m_renderArea,
        .clearValueCount = clearValues.size(),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    // Set the rendering viewport, but keep in mind that it will be reversed
    // after rendering (because of Vulkan's coordinate system). This sounds
    // counter-intuitive at first, but it makes things easier when applying shadows,
    // since we don't need to transform coordinate (see LocalLightingPass).
    const auto viewport = VkViewport {
        .width = float( m_renderArea.extent.width ),
        .height = float( m_renderArea.extent.height ),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

    const auto scissor = m_renderArea;
    vkCmdSetScissor( commandBuffer, 0, 1, &scissor );

    // Set depth bias (aka "Polygon offset")
    // Required to avoid shadow mapping artifacts
    vkCmdSetDepthBias(
        commandBuffer,
        // Constant depth bias factor (always applied)
        1.25f,
        0.0f,
        // Slope depth bias factor, applied depending on polygon's slope
        1.75f );

    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getHandle() );

    if ( light != nullptr ) {
        light->getShadowMap()->setLightProjectionMatrix( 0, perspective( 45.0f, 1.0f, 1.0f, 100.0f ) * light->getWorld().invMat );
        light->getShadowMap()->setLightProjectionMatrix( 1, perspective( 45.0f, 1.0f, 1.0f, 100.0f ) * light->getWorld().invMat );
        light->getShadowMap()->setLightProjectionMatrix( 2, perspective( 45.0f, 1.0f, 1.0f, 100.0f ) * light->getWorld().invMat );
        light->getShadowMap()->setLightProjectionMatrix( 3, perspective( 45.0f, 1.0f, 1.0f, 100.0f ) * light->getWorld().invMat );
        light->getShadowMap()->setLightViewMatrix( Matrix4::Constants::IDENTITY );

        if ( m_renderPassObjects.uniforms != nullptr ) {
            m_renderPassObjects.uniforms->setValue(
                RenderPassObjects::Uniforms {
                    .view = light->getShadowMap()->getLightViewMatrix(),
                    .proj = light->getShadowMap()->getLightProjectionMatrix( 0 ),
                } );
            getRenderDevice()->update( m_renderPassObjects.uniforms.get() );
        }

        renderables.eachGeometry(
            [ & ]( Geometry *geometry ) {
                if ( auto ms = geometry->getComponent< MaterialComponent >() ) {
                    if ( auto material = ms->first() ) {
                        vkCmdBindDescriptorSets(
                            commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipeline->getPipelineLayout(),
                            0,
                            1,
                            &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
                            0,
                            nullptr );

                        bindGeometryDescriptors( commandBuffer, currentFrameIndex, geometry );

                        drawPrimitive( commandBuffer, currentFrameIndex, geometry->anyPrimitive() );
                    }
                }
            } );
    }

    vkCmdEndRenderPass( commandBuffer );
}

void ShadowPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto extent = VkExtent2D {
        .width = 2048,
        .height = 2048,
    };

    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = extent,
    };

    createFramebufferAttachment( "Scene/Shadow", extent, VK_FORMAT_D32_SFLOAT, m_shadowAttachment );

    auto attachments = std::array< VkAttachmentDescription, 1 > {
        VkAttachmentDescription {
            .format = m_shadowAttachment.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
        },
    };

    auto depthReferences = std::array< VkAttachmentReference, 1 > {
        VkAttachmentReference {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        },
    };

    auto subpass = VkSubpassDescription {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 0,
        .pColorAttachments = nullptr,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = depthReferences.data(),
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    auto dependencies = std::array< VkSubpassDependency, 2 > {
        VkSubpassDependency {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        },
        VkSubpassDependency {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        }
    };

    auto createInfo = VkRenderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = crimild::UInt32( dependencies.size() ),
        .pDependencies = dependencies.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateRenderPass(
            getRenderDevice()->getHandle(),
            &createInfo,
            nullptr,
            &m_renderPass ) );

    // Should we have only 1 framebuffer?
    m_framebuffers.resize( getRenderDevice()->getSwapchainImageViews().size() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        auto attachments = std::array< VkImageView, 1 > {
            m_shadowAttachment.imageView,
        };

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = m_renderPass,
            .attachmentCount = uint32_t( attachments.size() ),
            .pAttachments = attachments.data(),
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        CRIMILD_VULKAN_CHECK(
            vkCreateFramebuffer(
                getRenderDevice()->getHandle(),
                &createInfo,
                nullptr,
                &m_framebuffers[ i ] ) );
    }

    createRenderPassObjects();
    createGeometryObjects();

    m_pipeline = [ & ] {
        auto program = crimild::alloc< ShaderProgram >();
        program->setShaders(
            Array< SharedPointer< Shader > > {
                crimild::alloc< Shader >(
                    Shader::Stage::VERTEX,
                    R"(
                        layout ( location = 0 ) in vec3 inPosition;

                        layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                            mat4 view;
                            mat4 proj;
                        };

                        layout( set = 1, binding = 0 ) uniform GeometryUniforms {
                            mat4 model;
                        };

                        void main()
                        {
                            gl_Position = proj * view * model * vec4( inPosition, 1.0 );
                        }
                    )" ),
            } );

        return std::make_unique< GraphicsPipeline >(
            getRenderDevice(),
            m_renderPass,
            std::vector< VkDescriptorSetLayout > {
                m_renderPassObjects.layout,
                m_geometryObjects.descriptorSetLayout,
            },
            program.get(),
            std::vector< VertexLayout > { VertexP3N3TC2::getLayout() },
            DepthStencilState {
                .depthCompareOp = CompareOp::LESS_OR_EQUAL,
            },
            RasterizationState {
                .depthBiasEnable = true,
                // .cullMode = CullMode::FRONT,
            },
            ColorBlendState {
                .enable = false,
            },
            0,
            std::vector< VkDynamicState > {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
                VK_DYNAMIC_STATE_DEPTH_BIAS,
            } );
    }();
}

void ShadowPass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;

    destroyGeometryObjects();
    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    destroyFramebufferAttachment( m_shadowAttachment );

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void ShadowPass::createRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderPassObjects.uniforms = [ & ] {
        auto ubo = std::make_unique< UniformBuffer >( RenderPassObjects::Uniforms {} );
        ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        getRenderDevice()->bind( ubo.get() );
        return ubo;
    }();

    const auto poolSizes = std::array< VkDescriptorPoolSize, 1 > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = uint32_t( poolSizes.size() ),
        .pPoolSizes = poolSizes.data(),
        .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_renderPassObjects.pool ) );

    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 1 > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = uint32_t( bindings.size() ),
        .pBindings = bindings.data(),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_renderPassObjects.layout ) );

    std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_renderPassObjects.layout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_renderPassObjects.pool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    m_renderPassObjects.descriptorSets.resize( getRenderDevice()->getSwapchainImageCount() );
    CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_renderPassObjects.descriptorSets.data() ) );

    for ( size_t i = 0; i < m_renderPassObjects.descriptorSets.size(); ++i ) {
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = getRenderDevice()->getHandle( m_renderPassObjects.uniforms.get(), i ),
            .offset = 0,
            .range = m_renderPassObjects.uniforms->getBufferView()->getLength(),
        };

        const auto writes = std::array< VkWriteDescriptorSet, 1 > {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_renderPassObjects.descriptorSets[ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo,
                .pImageInfo = nullptr,
                .pTexelBufferView = nullptr,
            },
        };
        vkUpdateDescriptorSets( getRenderDevice()->getHandle(), writes.size(), writes.data(), 0, nullptr );
    }
}

void ShadowPass::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_renderPassObjects.layout, nullptr );
    m_renderPassObjects.layout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( getRenderDevice()->getHandle(), m_renderPassObjects.pool, nullptr );
    m_renderPassObjects.pool = VK_NULL_HANDLE;

    getRenderDevice()->unbind( m_renderPassObjects.uniforms.get() );
    m_renderPassObjects.uniforms = nullptr;
}

void ShadowPass::createGeometryObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto layoutBinding = VkDescriptorSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr,
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &layoutBinding,
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_geometryObjects.descriptorSetLayout ) );
}

void ShadowPass::bindGeometryDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, Geometry *geometry ) noexcept
{
    if ( !m_geometryObjects.descriptorSets.contains( geometry ) ) {
        VkDescriptorPoolSize poolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        };

        auto poolCreateInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
            .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        };

        CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_geometryObjects.descriptorPools[ geometry ] ) );

        m_geometryObjects.uniforms[ geometry ] = std::make_unique< UniformBuffer >( Matrix4 {} );
        getRenderDevice()->bind( m_geometryObjects.uniforms[ geometry ].get() );

        std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_geometryObjects.descriptorSetLayout );

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_geometryObjects.descriptorPools[ geometry ],
            .descriptorSetCount = uint32_t( layouts.size() ),
            .pSetLayouts = layouts.data(),
        };

        m_geometryObjects.descriptorSets[ geometry ].resize( getRenderDevice()->getSwapchainImageCount() );
        CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_geometryObjects.descriptorSets[ geometry ].data() ) );

        for ( size_t i = 0; i < m_geometryObjects.descriptorSets[ geometry ].size(); ++i ) {
            const auto bufferInfo = VkDescriptorBufferInfo {
                .buffer = getRenderDevice()->getHandle( m_geometryObjects.uniforms[ geometry ].get(), i ),
                .offset = 0,
                .range = m_geometryObjects.uniforms[ geometry ]->getBufferView()->getLength(),
            };

            const auto descriptorWrite = VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_geometryObjects.descriptorSets[ geometry ][ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo,
                .pImageInfo = nullptr,
                .pTexelBufferView = nullptr,
            };

            vkUpdateDescriptorSets( getRenderDevice()->getHandle(), 1, &descriptorWrite, 0, nullptr );
        }
    }

    m_geometryObjects.uniforms[ geometry ]->setValue( geometry->getWorld().mat );
    getRenderDevice()->update( m_geometryObjects.uniforms[ geometry ].get() );

    vkCmdBindDescriptorSets(
        cmds,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getPipelineLayout(),
        1,
        1,
        &m_geometryObjects.descriptorSets[ geometry ][ currentFrameIndex ],
        0,
        nullptr );
}

void ShadowPass::destroyGeometryObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_geometryObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_geometryObjects.descriptorSetLayout, nullptr );
    m_geometryObjects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : m_geometryObjects.descriptorPools ) {
        vkDestroyDescriptorPool( getRenderDevice()->getHandle(), it.second, nullptr );
    }
    m_geometryObjects.descriptorPools.clear();

    for ( auto &it : m_geometryObjects.uniforms ) {
        getRenderDevice()->unbind( it.second.get() );
    }
    m_geometryObjects.uniforms.clear();
}

void ShadowPass::drawPrimitive( VkCommandBuffer cmds, Index currentFrameIndex, Primitive *primitive ) noexcept
{
    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                VkBuffer buffers[] = { getRenderDevice()->bind( vertices.get() ) };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( cmds, i, 1, buffers, offsets );
            }
        } );

    UInt32 instanceCount = 1;
    // if ( instanceData != nullptr ) {
    //     instanceCount = UInt32( instanceData->getVertexCount() );
    //     bindVertexBuffer( get_ptr( instanceData ), vboIndex++ );
    // }

    if ( instanceCount == 0 ) {
        CRIMILD_LOG_WARNING( "Instance count must be greater than zero. Primitive will not be rendered" );
        return;
    }

    auto indices = primitive->getIndices();
    if ( indices != nullptr ) {
        vkCmdBindIndexBuffer(
            cmds,
            getRenderDevice()->bind( indices ),
            0,
            utils::getIndexType( crimild::get_ptr( indices ) ) );
        vkCmdDrawIndexed( cmds, indices->getIndexCount(), instanceCount, 0, 0, 0 );
    } else if ( primitive->getVertexData().size() > 0 ) {
        auto vertices = primitive->getVertexData()[ 0 ];
        if ( vertices != nullptr && vertices->getVertexCount() > 0 ) {
            vkCmdDraw( cmds, vertices->getVertexCount(), 1, 0, 0 );
        }
    }
}
