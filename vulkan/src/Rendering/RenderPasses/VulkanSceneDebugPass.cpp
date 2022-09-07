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

#include "Rendering/RenderPasses/VulkanSceneDebugPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Debug/DebugDrawManager.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/UnlitMaterial.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/Apply.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

class DebugVisitor : public NodeVisitor {
public:
    explicit DebugVisitor( Camera *camera ) noexcept
        : m_camera( camera )
    {
        // no-op
    }

    ~DebugVisitor( void ) = default;

    virtual void visitNode( Node *node ) override
    {
        node->forEachComponent(
            [ & ]( auto cmp ) {
                if ( cmp != nullptr ) {
                    cmp->renderDebugInfo( nullptr, m_camera );
                }
            }
        );
    }

    virtual void visitGroup( Group *group ) override
    {
        visitNode( group );

        NodeVisitor::visitGroup( group );
    }

    virtual void visitCamera( Camera *camera ) override
    {
        NodeVisitor::visitCamera( camera );

        const auto P = location( camera->getWorld() );
        const auto D = forward( camera->getWorld() );
        const auto R = right( camera->getWorld() );
        const auto U = up( camera->getWorld() );
        const auto F = camera->getFrustum();

        const auto N0 = P + normalize( F.getDMin() * D + F.getRMin() * R + F.getUMax() * U );
        const auto N1 = P + normalize( F.getDMin() * D + F.getRMin() * R + F.getUMin() * U );
        const auto N2 = P + normalize( F.getDMin() * D + F.getRMax() * R + F.getUMin() * U );
        const auto N3 = P + normalize( F.getDMin() * D + F.getRMax() * R + F.getUMax() * U );

        const auto color = ColorRGB { 0, 1, 1 };
        DebugDrawManager::addLine( P, N0, color );
        DebugDrawManager::addLine( P, N1, color );
        DebugDrawManager::addLine( P, N2, color );
        DebugDrawManager::addLine( P, N3, color );
        DebugDrawManager::addLine( N0, N1, color );
        DebugDrawManager::addLine( N1, N2, color );
        DebugDrawManager::addLine( N2, N3, color );
        DebugDrawManager::addLine( N3, N0, color );
    }

    virtual void visitLight( Light *light ) override
    {
        NodeVisitor::visitLight( light );

        switch ( light->getType() ) {
            case Light::Type::POINT: {
                const auto P = location( light->getWorld() );
                DebugDrawManager::addCircle(
                    P,
                    forward( m_camera->getWorld() ),
                    light->getRadius(),
                    ColorRGB { 1, 1, 0 }
                );
                break;
            }

            case Light::Type::DIRECTIONAL: {
                const auto P = location( light->getWorld() );
                DebugDrawManager::addCircle(
                    P,
                    forward( m_camera->getWorld() ),
                    0.25f,
                    ColorRGB { 1, 1, 0 }
                );
                DebugDrawManager::addLine(
                    P,
                    P + 3.0 * forward( light->getWorld() ),
                    ColorRGB { 1, 1, 0 }
                );
                break;
            }

            default:
                break;
        }
    }

private:
    Camera *m_camera = nullptr;
};

// Vulkan spec only requires a minimum of 128 bytes. Anything larger should
// use normal uniforms instead.
struct PerModelUniforms {
    alignas( 16 ) ColorRGB color;
    alignas( 16 ) Matrix4 world;
};

SceneDebugPass::SceneDebugPass( RenderDevice *renderDevice ) noexcept
    : RenderPassBase( renderDevice ),
      m_program(
          [ & ] {
              auto program = std::make_unique< ShaderProgram >();
              program->setShaders(
                  Array< SharedPointer< Shader > > {
                      crimild::alloc< Shader >(
                          Shader::Stage::VERTEX,
                          R"(
                            layout ( location = 0 ) in vec3 inPosition;
                            layout ( location = 1 ) in vec3 inNormal;
                            layout ( location = 2 ) in vec2 inTexCoord;

                            layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                mat4 view;
                                mat4 proj;
                            };

                            layout ( push_constant ) uniform PerModelUniforms {
                                vec3 color;
                                mat4 model;
                            };

                            layout ( location = 0 ) out vec2 outTexCoord;
                            layout ( location = 1 ) out vec3 outColor;

                            void main()
                            {
                                gl_Position = proj * view * model * vec4( inPosition, 1.0 );
                                outTexCoord = inTexCoord;
                                outColor = color;
                            }
                        )"
                      ),
                      crimild::alloc< Shader >(
                          Shader::Stage::FRAGMENT,
                          R"(
                            layout( location = 0 ) in vec2 inTexCoord;
                            layout( location = 1 ) in vec3 inColor;

                            layout( location = 0 ) out vec4 outColor;

                            void main()
                            {
                                outColor = vec4( inColor, 1.0 );
                            }
                        )"
                      ) }
              );
              return program;
          }()
      )
{
    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = {
            .width = 1024,
            .height = 1024,
        },
    };

    init();
}

SceneDebugPass::~SceneDebugPass( void ) noexcept
{
    clear();
}

Event SceneDebugPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            m_renderArea.extent = {
                .width = uint32_t( e.extent.width ),
                .height = uint32_t( e.extent.height ),
            };
            clear();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void SceneDebugPass::render( Node *scene, Camera *camera ) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    const auto clearValues = std::array< VkClearValue, 2 > {
        VkClearValue {
            .color = {
                .float32 = {
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f,
                },
            },
        },
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
        .clearValueCount = static_cast< crimild::UInt32 >( clearValues.size() ),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    if ( scene != nullptr && camera != nullptr ) {
        // Set correct aspect ratio for camera before rendering
        camera->setAspectRatio( float( m_renderArea.extent.width ) / float( m_renderArea.extent.height ) );

        if ( m_renderPassObjects.uniforms != nullptr ) {
            m_renderPassObjects.uniforms->setValue(
                RenderPassObjects::Uniforms {
                    .view = camera->getViewMatrix(),
                    .proj = camera->getProjectionMatrix() }
            );
            getRenderDevice()->update( m_renderPassObjects.uniforms.get() );
        }

        scene->perform( DebugVisitor( camera ) );

        m_inFlightPrimitives[ currentFrameIndex ].clear();

        DebugDrawManager::eachRenderable(
            [ & ]( auto renderable ) {
                vkCmdBindPipeline(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_pipeline->getHandle()
                );

                vkCmdBindDescriptorSets(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_pipeline->getPipelineLayout(),
                    0,
                    1,
                    &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
                    0,
                    nullptr
                );

                PerModelUniforms model;
                model.world = renderable.world.mat;
                model.color = renderable.color;
                vkCmdPushConstants(
                    commandBuffer,
                    m_pipeline->getPipelineLayout(),
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof( PerModelUniforms ),
                    &model
                );

                auto primitive = renderable.primitive;
                drawPrimitive( commandBuffer, currentFrameIndex, primitive.get() );

                // Keep track of primitive, since DebugDrawManager might destroy it when resetting
                m_inFlightPrimitives[ currentFrameIndex ].insert( primitive );
            }
        );

        DebugDrawManager::reset();
    }

    vkCmdEndRenderPass( commandBuffer );
}

void SceneDebugPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    const auto depthFormat = VK_FORMAT_D32_SFLOAT;

    const auto extent = m_renderArea.extent;

    auto attachments = std::array< VkAttachmentDescription, 2 > {
        VkAttachmentDescription {
            .format = colorFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
        VkAttachmentDescription {
            .format = depthFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        }
    };

    auto colorReferences = std::array< VkAttachmentReference, 1 > {
        VkAttachmentReference {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
    };

    auto depthStencilReferences = std::array< VkAttachmentReference, 1 > {
        VkAttachmentReference {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        },
    };

    auto subpass = VkSubpassDescription {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = crimild::UInt32( colorReferences.size() ),
        .pColorAttachments = colorReferences.data(),
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = depthStencilReferences.data(),
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    auto dependencies = std::array< VkSubpassDependency, 2 > {
        VkSubpassDependency {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        },
        VkSubpassDependency {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
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
            &m_renderPass
        )
    );

    getRenderDevice()->createFramebufferAttachment( "Scene/Debug", extent, colorFormat, m_colorAttachment );
    getRenderDevice()->createFramebufferAttachment( "Scene/Debug/Depth", extent, depthFormat, m_depthAttachment );

    m_framebuffers.resize( getRenderDevice()->getSwapchainImageCount() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        auto attachments = std::array< VkImageView, 2 > {
            m_colorAttachment.imageViews[ i ],
            m_depthAttachment.imageViews[ i ],
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
                &m_framebuffers[ i ]
            )
        );
    }

    createRenderPassObjects();

    const auto viewport = ViewportDimensions::fromExtent( m_renderArea.extent.width, m_renderArea.extent.height );

    m_pipeline = std::make_unique< GraphicsPipeline >(
        getRenderDevice(),
        m_renderPass,
        GraphicsPipeline::Descriptor {
            .primitiveType = Primitive::Type::LINES,
            .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                m_renderPassObjects.layout,
            },
            .program = m_program.get(),
            .vertexLayouts = std::vector< VertexLayout > { VertexLayout::P3_N3_TC2 },
            .viewport = viewport,
            .scissor = viewport,
            .pushConstantRanges = {
                VkPushConstantRange {
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof( PerModelUniforms ),
                },
            },
        }
    );

    m_inFlightPrimitives.resize( getRenderDevice()->getSwapchainImageCount() );
}

void SceneDebugPass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;

    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    getRenderDevice()->destroyFramebufferAttachment( m_colorAttachment );
    getRenderDevice()->destroyFramebufferAttachment( m_depthAttachment );

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void SceneDebugPass::createRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderPassObjects.uniforms = [ & ] {
        auto ubo = std::make_unique< UniformBuffer >( RenderPassObjects::Uniforms {} );
        ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        getRenderDevice()->bind( ubo.get() );
        return ubo;
    }();

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

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_renderPassObjects.pool ) );

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

        const auto descriptorWrite = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = m_renderPassObjects.descriptorSets[ i ],
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

void SceneDebugPass::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_renderPassObjects.layout, nullptr );
    m_renderPassObjects.layout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( getRenderDevice()->getHandle(), m_renderPassObjects.pool, nullptr );
    m_renderPassObjects.pool = VK_NULL_HANDLE;

    getRenderDevice()->unbind( m_renderPassObjects.uniforms.get() );
    m_renderPassObjects.uniforms = nullptr;
}

void SceneDebugPass::drawPrimitive( VkCommandBuffer cmds, Index currentFrameIndex, Primitive *primitive ) noexcept
{
    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                VkBuffer buffers[] = { getRenderDevice()->bind( vertices.get() ) };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( cmds, i, 1, buffers, offsets );
            }
        }
    );

    auto indices = primitive->getIndices();
    if ( indices != nullptr ) {
        vkCmdBindIndexBuffer(
            cmds,
            getRenderDevice()->bind( indices ),
            0,
            utils::getIndexType( crimild::get_ptr( indices ) )
        );
        vkCmdDrawIndexed( cmds, indices->getIndexCount(), 1, 0, 0, 0 );
    } else if ( primitive->getVertexData().size() > 0 ) {
        auto vertices = primitive->getVertexData()[ 0 ];
        if ( vertices != nullptr && vertices->getVertexCount() > 0 ) {
            vkCmdDraw( cmds, vertices->getVertexCount(), 1, 0, 0 );
        }
    }
}
