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

#include "Rendering/RenderPasses/VulkanUnlitPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Materials/UnlitMaterial.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/ApplyToGeometries.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

UnlitPass::UnlitPass(
    RenderDevice *renderDevice,
    const FramebufferAttachment *colorAttachment,
    const FramebufferAttachment *depthAttachment
) noexcept
    : RenderPassBase( renderDevice ),
      m_colorAttachment( colorAttachment ),
      m_depthAttachment( depthAttachment )
{
    init();
}

UnlitPass::~UnlitPass( void ) noexcept
{
    clear();
}

Event UnlitPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::SCENE_CHANGED:
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

void UnlitPass::render( const SceneRenderState::RenderableSet< UnlitMaterial > &sceneRenderables, const Camera *camera ) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    auto renderPassInfo = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[ currentFrameIndex ],
        .renderArea = m_renderArea,
        .clearValueCount = 0,
        .pClearValues = nullptr,
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    if ( camera != nullptr ) {
        if ( m_renderPassObjects.uniforms != nullptr ) {
            m_renderPassObjects.uniforms->setValue(
                RenderPassObjects::Uniforms {
                    .view = camera->getViewMatrix(),
                    .proj = camera->getProjectionMatrix() }
            );
            getRenderDevice()->update( m_renderPassObjects.uniforms.get() );
        }
    }

    for ( auto &[ material, primitives ] : sceneRenderables ) {
        for ( auto &[ primitive, renderables ] : primitives ) {
            for ( auto &renderable : renderables ) {
                bind( material );

                vkCmdBindPipeline(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_materialObjects.pipelines[ material ]->getHandle()
                );

                vkCmdBindDescriptorSets(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_materialObjects.pipelines[ material ]->getPipelineLayout(),
                    0,
                    1,
                    &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
                    0,
                    nullptr
                );

                vkCmdBindDescriptorSets(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_materialObjects.pipelines[ material ]->getPipelineLayout(),
                    1,
                    1,
                    &m_materialObjects.descriptorSets[ material ][ currentFrameIndex ],
                    0,
                    nullptr
                );

                // Vulkan spec only requires a minimum of 128 bytes. Anything larger should
                // use normal uniforms instead.
                vkCmdPushConstants(
                    commandBuffer,
                    m_materialObjects.pipelines[ material ]->getPipelineLayout(),
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof( SceneRenderState::Renderable ),
                    &renderable
                );

                drawPrimitive( commandBuffer, primitive );
            }
        }
    }

    vkCmdEndRenderPass( commandBuffer );
}

void UnlitPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    if ( m_colorAttachment == nullptr && m_depthAttachment == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid attachment for unlit pass" );
        exit( -1 );
    }

    m_renderArea = VkRect2D {
        .offset = { 0, 0 },
        .extent =
            m_colorAttachment != nullptr
                ? m_colorAttachment->extent
                : m_depthAttachment->extent,
    };

    const auto extent = m_renderArea.extent;

    std::vector< VkAttachmentDescription > attachments;
    std::vector< VkAttachmentReference > colorReferences;
    std::vector< VkAttachmentReference > depthStencilReferences;

    if ( m_colorAttachment != nullptr ) {
        attachments.push_back(
            VkAttachmentDescription {
                .format = m_colorAttachment->format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            }
        );
        colorReferences.push_back(
            VkAttachmentReference {
                .attachment = 0,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            }
        );
    }

    if ( m_depthAttachment != nullptr ) {
        attachments.push_back(
            VkAttachmentDescription {
                .format = m_depthAttachment->format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            }
        );
        depthStencilReferences.push_back(
            VkAttachmentReference {
                .attachment = uint32_t( m_colorAttachment != nullptr ? 1 : 0 ),
                .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            }
        );
    }

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

    m_framebuffers.resize( getRenderDevice()->getSwapchainImageCount() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        std::vector< VkImageView > imageViews;
        if ( m_colorAttachment != nullptr ) {
            imageViews.push_back( *m_colorAttachment->imageViews[ i ] );
        }
        if ( m_depthAttachment != nullptr ) {
            imageViews.push_back( *m_depthAttachment->imageViews[ i ] );
        }

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = m_renderPass,
            .attachmentCount = uint32_t( imageViews.size() ),
            .pAttachments = imageViews.data(),
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
    createMaterialObjects();
}

void UnlitPass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    destroyMaterialObjects();
    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void UnlitPass::createRenderPassObjects( void ) noexcept
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
        .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
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
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = nullptr,
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = nullptr,
        };

        vkUpdateDescriptorSets( getRenderDevice()->getHandle(), 1, &descriptorWrite, 0, nullptr );
    }
}

void UnlitPass::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_renderPassObjects.layout, nullptr );
    m_renderPassObjects.layout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( getRenderDevice()->getHandle(), m_renderPassObjects.pool, nullptr );
    m_renderPassObjects.pool = VK_NULL_HANDLE;

    getRenderDevice()->unbind( m_renderPassObjects.uniforms.get() );
    m_renderPassObjects.uniforms = nullptr;
}

void UnlitPass::createMaterialObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 2 > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
        VkDescriptorSetLayoutBinding {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = uint32_t( bindings.size() ),
        .pBindings = bindings.data(),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_materialObjects.descriptorSetLayout ) );
}

void UnlitPass::bind( const Material *aMaterial ) noexcept
{
    const auto material = static_cast< const UnlitMaterial * >( aMaterial );

    // TODO: Create pipelines (and maybe pools/layouts) per material type, not material instance
    // That way we can reuse some of them (but uniforms must be assigned per instance)

    if ( m_materialObjects.pipelines.contains( material ) ) {
        // Already bound
        // TODO: This should be handled in a different way. What if texture changes?
        // Also, update only when material changes.
        m_materialObjects.uniforms[ material ]->setValue( material->getColor() );
        getRenderDevice()->update( m_materialObjects.uniforms[ material ].get() );
        return;
    }

    // create pipeline
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

                    layout ( push_constant ) uniform RenderableUniforms {
                        mat4 model;
                    };

                    layout ( location = 0 ) out vec3 outWorldPosition;
                    layout ( location = 1 ) out vec2 outTexCoord;

                    struct Vertex {
                        vec3 position;
                        vec3 worldPosition;
                        vec3 worldNormal;
                        vec2 texCoord;
                    };

                    #include <vert_main>

                    void main()
                    {
                        Vertex vertex;

                        vertex.position = inPosition;
                        vertex.worldPosition = ( model * vec4( inPosition, 1.0 ) ).xyz;
                        vertex.worldNormal = normalize( inverse( transpose( mat3( model ) ) ) * inNormal );
                        vertex.texCoord = inTexCoord;

                        gl_Position = vert_main( vertex, proj, view, model );

                        outWorldPosition = vertex.worldPosition;
                        outTexCoord = vertex.texCoord;
                    }
                )"
            ),
            crimild::alloc< Shader >(
                Shader::Stage::FRAGMENT,
                R"(
                    layout ( location = 0 ) in vec3 inWorldPosition;
                    layout ( location = 1 ) in vec2 inTexCoord;

                    layout( set = 1, binding = 0 ) uniform MaterialUniform {
                        vec4 color;
                    } uMaterial;

                    layout( set = 1, binding = 1 ) uniform sampler2D uColorMap;

                    layout ( location = 0 ) out vec4 outFragColor;

                    struct Fragment {
                        vec3 color;
                        float opacity;
                        vec3 worldPosition;
                        vec2 texCoord;
                    };

                    #include <frag_main>

                    void main()
                    {
                        Fragment frag;

                        frag.color = ( uMaterial.color * texture( uColorMap, inTexCoord ) ).rgb;
                        frag.opacity = 1.0;
                        frag.worldPosition = inWorldPosition;
                        frag.texCoord = inTexCoord;

                        frag_main( frag );

                        outFragColor = vec4( frag.color, frag.opacity );
                    }
                )"
            ) 
        }
    );

    if ( auto program = material->getProgram() ) {
        getRenderDevice()->getShaderCompiler().addChunks( program->getShaders() );
    }

    const auto viewport = ViewportDimensions::fromExtent( m_renderArea.extent.width, m_renderArea.extent.height );

    auto pipeline = std::make_unique< GraphicsPipeline >(
        getRenderDevice(),
        m_renderPass,
        GraphicsPipeline::Descriptor {
            .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                m_renderPassObjects.layout,
                m_materialObjects.descriptorSetLayout,
            },
            .program = program.get(),
            .vertexLayouts = { VertexLayout::P3_N3_TC2 },
            .viewport = viewport,
            .scissor = viewport,
            .pushConstantRanges = {
                VkPushConstantRange {
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof( SceneRenderState::Renderable ),
                },
            },
        }
    );
    m_materialObjects.pipelines[ material ] = std::move( pipeline );

    // create descriptors
    const auto poolSizes = std::array< VkDescriptorPoolSize, 2 > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        },
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        .poolSizeCount = uint32_t( poolSizes.size() ),
        .pPoolSizes = poolSizes.data(),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_materialObjects.descriptorPools[ material ] ) );

    m_materialObjects.uniforms[ material ] = std::make_unique< UniformBuffer >( material->getColor() );
    getRenderDevice()->bind( m_materialObjects.uniforms[ material ].get() );

    std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_materialObjects.descriptorSetLayout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_materialObjects.descriptorPools[ material ],
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    m_materialObjects.descriptorSets[ material ].resize( getRenderDevice()->getSwapchainImageCount() );
    CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_materialObjects.descriptorSets[ material ].data() ) );

    auto imageView = getRenderDevice()->bind( material->getTexture()->imageView.get() );
    auto sampler = getRenderDevice()->bind( material->getTexture()->sampler.get() );

    for ( size_t i = 0; i < m_materialObjects.descriptorSets[ material ].size(); ++i ) {
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = getRenderDevice()->getHandle( m_materialObjects.uniforms[ material ].get(), i ),
            .offset = 0,
            .range = m_materialObjects.uniforms[ material ]->getBufferView()->getLength(),
        };

        const auto imageInfo = VkDescriptorImageInfo {
            .sampler = sampler,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        const auto writes = std::array< VkWriteDescriptorSet, 2 > {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_materialObjects.descriptorSets[ material ][ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo = nullptr,
                .pBufferInfo = &bufferInfo,
                .pTexelBufferView = nullptr,
            },
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_materialObjects.descriptorSets[ material ][ i ],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr,
            },
        };
        vkUpdateDescriptorSets( getRenderDevice()->getHandle(), writes.size(), writes.data(), 0, nullptr );
    }
}

void UnlitPass::destroyMaterialObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_materialObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_materialObjects.descriptorSetLayout, nullptr );
    m_materialObjects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : m_materialObjects.descriptorPools ) {
        vkDestroyDescriptorPool( getRenderDevice()->getHandle(), it.second, nullptr );
    }
    m_materialObjects.descriptorPools.clear();

    for ( auto &it : m_materialObjects.uniforms ) {
        getRenderDevice()->unbind( it.second.get() );
    }
    m_materialObjects.uniforms.clear();

    m_materialObjects.pipelines.clear();
}

void UnlitPass::drawPrimitive( VkCommandBuffer cmds, const Primitive *primitive ) noexcept
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
