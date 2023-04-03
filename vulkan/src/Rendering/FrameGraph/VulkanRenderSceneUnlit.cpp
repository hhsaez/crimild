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
 *     * Neither the name of the copyright holder nor the
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

#include "Rendering/FrameGraph/VulkanRenderSceneUnlit.hpp"

#include "Rendering/Materials/UnlitMaterial.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderDeviceCache.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "SceneGraph/Camera.hpp"

using namespace crimild;
using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

RenderSceneUnlit::RenderSceneUnlit(
    RenderDevice *device,
    const VkExtent2D &extent,
    std::shared_ptr< RenderTarget > const &depthTarget,
    std::shared_ptr< RenderTarget > const &colorTarget
) noexcept
    : RenderBase( device, "RenderSceneUnlit", extent ),
      WithCommandBuffer(
          crimild::alloc< CommandBuffer >(
              getRenderDevice(),
              getName() + "/CommandBuffer",
              VK_COMMAND_BUFFER_LEVEL_PRIMARY
          )
      ),
      m_depthTarget( depthTarget ),
      m_colorTarget( colorTarget )
{
    std::vector< std::shared_ptr< RenderTarget > > renderTargets;
    if ( m_depthTarget != nullptr ) {
        renderTargets.push_back( m_depthTarget );
    }
    if ( m_colorTarget != nullptr ) {
        renderTargets.push_back( m_colorTarget );
    }

    m_resources.common.renderPass = crimild::alloc< RenderPass >(
        getRenderDevice(),
        getName() + "/RenderPass",
        renderTargets,
        VK_ATTACHMENT_LOAD_OP_LOAD
    );

    m_resources.common.framebuffer = crimild::alloc< Framebuffer >(
        getRenderDevice(),
        getName() + "/Framebuffer",
        getExtent(),
        m_resources.common.renderPass,
        renderTargets
    );

    m_resources.common.uniforms = [ & ] {
        auto uniforms = crimild::alloc< UniformBuffer >( Resources::Common::UniformData {} );
        uniforms->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        device->getCache()->bind( uniforms.get() );
        return uniforms;
    }();

    m_resources.common.descriptorSet = crimild::alloc< DescriptorSet >(
        getRenderDevice(),
        getName() + "/Common/DescriptorSet",
        std::vector< Descriptor > {
            Descriptor {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .buffer = device->getCache()->bind( m_resources.common.uniforms.get() ),
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            },
        }
    );
}

void RenderSceneUnlit::bindMaterial( const UnlitMaterial *material ) noexcept
{
    if ( m_resources.materials.contains( material ) ) {
        // Already bound
        // TODO: This should be handled in a different way. What if texture changes?
        // Also, update only when material changes.
        m_resources.materials[ material ].uniforms->setValue( material->getColor() );
        return;
    }

    std::string name = getName();
    name += "/" + ( !material->getName().empty() ? material->getName() : "Material" );

    auto renderCache = getRenderDevice()->getCache();

    m_resources.materials[ material ].uniforms = [ & ] {
        auto uniforms = crimild::alloc< UniformBuffer >( material->getColor() );
        uniforms->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        return uniforms;
    }();

    m_resources.materials[ material ].descriptorSet = crimild::alloc< DescriptorSet >(
        getRenderDevice(),
        name + "/DescriptorSet",
        std::vector< Descriptor > {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .buffer = renderCache->bind( m_resources.materials[ material ].uniforms.get() ),
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = renderCache->bind( material->getTexture()->imageView.get() ),
                .sampler = renderCache->bind( material->getTexture()->sampler.get() ),
            },
        }
    );

    // create pipeline
    auto program = crimild::alloc< ShaderProgram >();
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
            ),
        }
    );

    // TODO: Only create a new pipeline for materials that provide custom shaders.
    // Otherwise, have a fallback one that can be reused.
    if ( auto program = material->getProgram() ) {
        getRenderDevice()->getShaderCompiler().addChunks( program->getShaders() );
    }

    const auto viewport = ViewportDimensions::fromExtent( getExtent().width, getExtent().height );

    auto pipeline = crimild::alloc< GraphicsPipeline >(
        getRenderDevice(),
        m_resources.common.renderPass->getHandle(),
        GraphicsPipeline::Descriptor {
            .primitiveType = Primitive::Type::TRIANGLES,
            .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                m_resources.common.descriptorSet->getDescriptorSetLayout()->getHandle(),
                m_resources.materials[ material ].descriptorSet->getDescriptorSetLayout()->getHandle(),
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

    getRenderDevice()->setObjectName( pipeline->getHandle(), name + "/Pipeline" );

    m_resources.materials[ material ].pipeline = pipeline;
}

void RenderSceneUnlit::render(
    const SceneRenderState::RenderableSet< UnlitMaterial > &sceneRenderables,
    const Camera *camera,
    SyncOptions const &options
) noexcept
{
    // Update camera uniforms
    if ( camera != nullptr ) {
        m_resources.common.uniforms->setValue(
            Resources::Common::UniformData {
                .view = camera->getViewMatrix(),
                .proj = camera->getProjectionMatrix(),
            }
        );
    }

    auto &cmds = getCommandBuffer();
    cmds->reset();

    cmds->begin( options );
    cmds->beginRenderPass( m_resources.common.renderPass, m_resources.common.framebuffer );

    // TODO: add support for instancing
    for ( auto &[ material, primitives ] : sceneRenderables ) {
        for ( auto &[ primitive, renderables ] : primitives ) {
            for ( auto &renderable : renderables ) {
                bindMaterial( material.get() );

                cmds->bindPipeline( m_resources.materials[ material.get() ].pipeline );
                cmds->bindDescriptorSet( 0, m_resources.common.descriptorSet );
                cmds->bindDescriptorSet( 1, m_resources.materials[ material.get() ].descriptorSet );

                // Vulkan spec only requires a minimum of 128 bytes. Anything larger should
                // use normal uniforms instead.
                cmds->pushConstants( VK_SHADER_STAGE_VERTEX_BIT, 0, renderable );

                cmds->drawPrimitive( primitive );
            }
        }
    }

    cmds->endRenderPass();
    cmds->end( options );

    getRenderDevice()->submitGraphicsCommands( cmds, options.wait, options.signal );
}
