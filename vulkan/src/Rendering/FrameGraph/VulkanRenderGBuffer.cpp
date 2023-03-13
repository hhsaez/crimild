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

#include "Rendering/FrameGraph/VulkanRenderGBuffer.hpp"

#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
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

using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

RenderGBuffer::RenderGBuffer(
    RenderDevice *device,
    const VkExtent2D &extent,
    const std::vector< std::shared_ptr< RenderTarget > > &renderTargets
) noexcept
    : RenderBase( device, "RenderGBuffer", extent ),
      m_renderTargets( renderTargets ),
      m_commandBuffer(
          crimild::alloc< CommandBuffer >(
              device,
              getName() + "/CommandBuffer",
              VK_COMMAND_BUFFER_LEVEL_PRIMARY
          )
      )
{
    createRenderPassResources();
    createMaterialResources();
}

RenderGBuffer::~RenderGBuffer( void ) noexcept
{
    destroyMaterialResources();
    destroyRenderPassResources();
}

void RenderGBuffer::onResize( void ) noexcept
{
    CRIMILD_LOG_TRACE();
    assert( false && "Method not implemented yet" );
}

void RenderGBuffer::createRenderPassResources( void ) noexcept
{
    m_resources.renderPass.renderPass = crimild::alloc< RenderPass >(
        getRenderDevice(),
        getName() + "/RenderPass",
        getRenderTargets(),
        VK_ATTACHMENT_LOAD_OP_CLEAR
    );

    m_resources.renderPass.framebuffer = crimild::alloc< Framebuffer >(
        getRenderDevice(),
        getName() + "/Framebuffer",
        getExtent(),
        m_resources.renderPass.renderPass,
        getRenderTargets()
    );

    m_resources.renderPass.uniforms = crimild::alloc< UniformBuffer >( Resources::RenderPassResources::UniformData {} );
    m_resources.renderPass.uniforms->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );

    m_resources.renderPass.descriptorSet = crimild::alloc< DescriptorSet >(
        getRenderDevice(),
        getName() + "/DescriptorSet",
        std::vector< Descriptor > {
            Descriptor {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .buffer = getRenderDevice()->getCache()->bind(
                    m_resources.renderPass.uniforms.get()
                ),
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
            },
        }
    );
}

void RenderGBuffer::destroyRenderPassResources( void ) noexcept
{
    m_resources.renderPass = {};
}

void RenderGBuffer::createMaterialResources( void ) noexcept
{
    // TODO?
}

void RenderGBuffer::bindMaterial( const materials::PrincipledBSDF *material ) noexcept
{
    if ( m_resources.materials.contains( material ) ) {
        // Already bound
        // TODO: This should be handled in a different way. What if texture changes?
        // Also, update only when material changes.
        // TODO: Update material uniforms
        return;
    }

    std::string name = getName();
    name += "/" + ( !material->getName().empty() ? material->getName() : "Material" );

    auto renderCache = getRenderDevice()->getCache();

    m_resources.materials[ material ].uniforms = crimild::alloc< UniformBuffer >( material->getProps() );

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
                .imageView = renderCache->bind( material->getAlbedoMap()->imageView.get() ),
                .sampler = renderCache->bind( material->getAlbedoMap()->sampler.get() ),
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

                    layout ( push_constant ) uniform GeometryUniforms {
                        mat4 model;
                    };

                    layout ( location = 0 ) out vec3 outPosition;
                    layout ( location = 1 ) out vec3 outNormal;
                    layout ( location = 2 ) out vec2 outTexCoord;
                    layout ( location = 3 ) out vec3 outScale;
                    layout ( location = 4 ) out vec3 outModelPosition;
                    layout ( location = 5 ) out vec3 outModelNormal;
                    layout ( location = 6 ) out vec3 outViewPosition;

                    void main()
                    {
                        gl_Position = proj * view * model * vec4( inPosition, 1.0 );

                        outPosition = ( model * vec4( inPosition, 1.0 ) ).xyz;
                        outNormal = inverse( transpose( mat3( model ) ) ) * inNormal;
                        outTexCoord = inTexCoord;

                        outScale = vec3(
                            length( model[ 0 ].xyz ),
                            length( model[ 1 ].xyz ),
                            length( model[ 2 ].xyz ) );

                        outModelPosition = inPosition;
                        outModelNormal = inNormal;

                        outViewPosition = ( view * model * vec4( inPosition, 1 ) ).xyz;
                    }
                )"
            ),
            crimild::alloc< Shader >(
                Shader::Stage::FRAGMENT,
                R"(
                    layout ( location = 0 ) in vec3 inPosition;
                    layout ( location = 1 ) in vec3 inNormal;
                    layout ( location = 2 ) in vec2 inTexCoord;
                    layout ( location = 3 ) in vec3 inScale;
                    layout ( location = 4 ) in vec3 inModelPosition;
                    layout ( location = 5 ) in vec3 inModelNormal;
                    layout ( location = 6 ) in vec3 inViewPosition;

                    layout( set = 1, binding = 0 ) uniform MaterialUniform
                    {
                        vec3 albedo;
                        float metallic;
                        float roughness;
                        float ambientOcclusion;
                        float transmission;
                        float indexOfRefraction;
                        vec3 emissive;
                    } uMaterial;

                    layout( set = 1, binding = 1 ) uniform sampler2D uAlbedoMap;

                    layout ( location = 0 ) out vec4 outAlbedo;
                    layout ( location = 1 ) out vec4 outPosition;
                    layout ( location = 2 ) out vec4 outNormal;
                    layout ( location = 3 ) out vec4 outMaterial;

                    struct Fragment {
                        vec3 albedo;
                        vec3 position;
                        vec3 modelPosition;
                        vec3 normal;
                        vec3 modelNormal;
                        vec2 uv;
                        vec3 scale;
                        float depth;
                        float metallic;
                        float roughness;
                        float ambientOcclusion;
                    };

                    #include <frag_main>

                    void main()
                    {
                        Fragment frag;

                        frag.albedo = uMaterial.albedo * pow( texture( uAlbedoMap, inTexCoord ).rgb, vec3( 2.2 ) );
                        frag.position = inPosition;
                        frag.normal = inNormal;
                        frag.uv = inTexCoord;
                        frag.modelPosition = inModelPosition;
                        frag.modelNormal = inModelNormal;
                        frag.scale = inScale;
                        frag.depth = gl_FragCoord.z;

                        frag.metallic = uMaterial.metallic;// * texture( uMetallicMap, inTexCoord ).r;
                        frag.roughness = uMaterial.roughness;// * texture( uRoughnessMap, inTexCoord ).r;
                        frag.ambientOcclusion = uMaterial.ambientOcclusion;// * texture( uAmbientOcclusionMap, inTexCoord ).r;

                        // Clamp metallic and roughness
                        frag.metallic = clamp( frag.metallic, 0.0, 1.0 );
                        frag.roughness = clamp( frag.roughness, 0.05, 0.999 );

                        frag_main( frag );

                        outAlbedo = vec4( frag.albedo, 1.0 );
                        outPosition = vec4( frag.position, inViewPosition.z );
                        outNormal = vec4( frag.normal, 1.0 );
                        outMaterial = vec4( frag.metallic, frag.roughness, frag.ambientOcclusion, 1.0 );
                    }
                )"
            ) }
    );

    if ( auto program = material->getProgram() ) {
        getRenderDevice()->getShaderCompiler().addChunks( program->getShaders() );
    }

    const auto viewport = ViewportDimensions::fromExtent( getExtent().width, getExtent().height );

    auto pipeline = crimild::alloc< GraphicsPipeline >(
        getRenderDevice(),
        m_resources.renderPass.renderPass->getHandle(),
        GraphicsPipeline::Descriptor {
            .primitiveType = Primitive::Type::TRIANGLES,
            .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                m_resources.renderPass.descriptorSet->getDescriptorSetLayout()->getHandle(),
                m_resources.materials[ material ].descriptorSet->getDescriptorSetLayout()->getHandle(),
            },
            .program = program.get(),
            .vertexLayouts = { VertexLayout::P3_N3_TC2 },
            .colorAttachmentCount = 4,
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

void RenderGBuffer::destroyMaterialResources( void ) noexcept
{
    m_resources.materials.clear();
}

void RenderGBuffer::invalidates( std::vector< std::shared_ptr< RenderTarget > > const &renderTargets ) noexcept
{
    for ( auto &rt : renderTargets ) {
        m_imagesToInvalidate.insert( rt->getImage() );
    }
}

void RenderGBuffer::flushes( std::vector< std::shared_ptr< RenderTarget > > const &renderTargets ) noexcept
{
    for ( auto &rt : renderTargets ) {
        m_imagesToFlush.insert( rt->getImage() );
    }
}

void RenderGBuffer::render(
    const SceneRenderState::RenderableSet< materials::PrincipledBSDF > &sceneRenderables,
    const Camera *camera
) noexcept
{
    if ( camera != nullptr ) {
        if ( m_resources.renderPass.uniforms != nullptr ) {
            m_resources.renderPass.uniforms->setValue(
                Resources::RenderPassResources::UniformData {
                    .view = camera->getViewMatrix(),
                    .proj = camera->getProjectionMatrix() }
            );
        }
    }

    auto cmds = getCommandBuffer();
    cmds->reset();

    cmds->begin();
    cmds->invalidate( m_imagesToInvalidate );
    cmds->beginRenderPass( m_resources.renderPass.renderPass, m_resources.renderPass.framebuffer );

    // TODO: add support for instancing
    for ( auto &[ material, primitives ] : sceneRenderables ) {
        for ( auto &[ primitive, renderables ] : primitives ) {
            for ( auto &renderable : renderables ) {
                bindMaterial( material.get() );

                cmds->bindPipeline( m_resources.materials[ material.get() ].pipeline );
                cmds->bindDescriptorSet( 0, m_resources.renderPass.descriptorSet );
                cmds->bindDescriptorSet( 1, m_resources.materials[ material.get() ].descriptorSet );

                // Vulkan spec only requires a minimum of 128 bytes. Anything larger should
                // use normal uniforms instead.
                cmds->pushConstants( VK_SHADER_STAGE_VERTEX_BIT, 0, renderable );

                cmds->drawPrimitive( primitive );
            }
        }
    }

    cmds->endRenderPass();
    cmds->flush( m_imagesToFlush );
    cmds->end();

    getRenderDevice()->submitGraphicsCommands( cmds );
}
