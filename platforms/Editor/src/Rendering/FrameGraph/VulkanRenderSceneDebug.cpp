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

#include "Rendering/FrameGraph/VulkanRenderSceneDebug.hpp"

#include "Crimild_Mathematics.hpp"
#include "Debug/DebugDrawManager.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Light.hpp"
#include "Simulation/Editor.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/NodeVisitor.hpp"

using namespace crimild;
using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

class DebugVisitor : public NodeVisitor {
public:
    explicit DebugVisitor( Camera *camera ) noexcept
        : m_camera( camera )
    {
        // no-op
    }

    ~DebugVisitor( void ) = default;

    virtual void visitNode( crimild::Node *node ) override
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

        const auto P = origin( camera->getWorld() );
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
                const auto P = origin( light->getWorld() );
                DebugDrawManager::addCircle(
                    P,
                    forward( m_camera->getWorld() ),
                    light->getRadius(),
                    ColorRGB { 1, 1, 0 }
                );
                DebugDrawManager::addLine(
                    P,
                    P + 3.0 * forward( light->getWorld() ),
                    ColorRGB { 1, 1, 0 }
                );
                break;
            }

            case Light::Type::SPOT: {
                const auto P = origin( light->getWorld() );
                DebugDrawManager::addCircle(
                    P,
                    forward( m_camera->getWorld() ),
                    light->getRadius(),
                    ColorRGB { 1, 1, 0 }
                );
                DebugDrawManager::addLine(
                    P,
                    P + 3.0 * forward( light->getWorld() ),
                    ColorRGB { 1, 1, 0 }
                );
                break;
            }

            case Light::Type::DIRECTIONAL: {
                const auto P = origin( light->getWorld() );
                DebugDrawManager::addCircle(
                    P,
                    forward( m_camera->getWorld() ),
                    0.25f,
                    ColorRGB { 1, 1, 0 }
                );
                DebugDrawManager::addLine(
                    P,
                    P + 10.0 * forward( light->getWorld() ),
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

RenderSceneDebug::RenderSceneDebug(
    RenderDevice *device,
    std::string name,
    std::shared_ptr< Camera > const &camera,
    std::shared_ptr< ImageView > const &output,
    SyncOptions const &options
) noexcept
    : RenderBase( device, name, output ),
      WithCommandBuffer(
          crimild::alloc< CommandBuffer >(
              getRenderDevice(),
              getName() + "/CommandBuffer",
              VK_COMMAND_BUFFER_LEVEL_PRIMARY
          )
      ),
      m_output( output ),
      m_camera( camera ),
      m_syncOptions( options ),
      m_outputTarget( crimild::alloc< RenderTarget >( device, getName() + "/Targets/Color", output ) )
{
    const auto extent = VkExtent2D {
        m_output->getExtent().width,
        m_output->getExtent().height,
    };

    auto renderTargets = std::vector< std::shared_ptr< RenderTarget > > { m_outputTarget };

    m_resources.renderPass = crimild::alloc< RenderPass >(
        getRenderDevice(),
        getName() + "/RenderPass",
        renderTargets,
        VK_ATTACHMENT_LOAD_OP_CLEAR
    );

    m_resources.framebuffer = crimild::alloc< Framebuffer >(
        getRenderDevice(),
        getName() + "/Framebuffer",
        extent,
        m_resources.renderPass,
        renderTargets
    );

    m_resources.pipeline = [ & ] {
        auto program = crimild::alloc< ShaderProgram >();
        program->setShaders(
            Array< SharedPointer< Shader > > {
                crimild::alloc< Shader >(
                    Shader::Stage::VERTEX,
                    R"(
                        layout ( location = 0 ) in vec3 inPosition;

                        layout( push_constant ) uniform Uniforms {
                            mat4 mvp;
                            vec3 color;
                        };

                        layout ( location = 0 ) out vec3 outColor;

                        void main()
                        {
                            gl_Position = mvp * vec4( inPosition, 1.0 );

                            outColor = color;
                        }
                    )"
                ),
                crimild::alloc< Shader >(
                    Shader::Stage::FRAGMENT,
                    R"(
                        layout ( location = 0 ) in vec3 inColor;

                        layout ( location = 0 ) out vec4 outColor;

                        void main()
                        {
                            outColor = vec4( inColor, 1.0 );
                        }
                    )"
                ),
            }
        );

        const auto viewport = ViewportDimensions::fromExtent( extent.width, extent.height );

        const auto pipelineDescriptor = GraphicsPipeline::Descriptor {
            .primitiveType = Primitive::Type::LINES,
            .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {},
            .program = program.get(),
            .vertexLayouts = { VertexLayout::P3_N3_TC2 },
            .viewport = viewport,
            .scissor = viewport,
            .pushConstantRanges = {
                VkPushConstantRange {
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof( PushConstantsData ),
                },
            },
        };
        auto pipeline = std::make_unique< GraphicsPipeline >(
            getRenderDevice(),
            m_resources.renderPass->getHandle(),
            pipelineDescriptor
        );
        getRenderDevice()->setObjectName( pipeline->getHandle(), getName() + "/Pipeline" );
        return pipeline;
    }();
}

void RenderSceneDebug::execute( void ) noexcept
{
    auto &cmds = getCommandBuffer();

    cmds->reset();
    cmds->begin( m_syncOptions );
    cmds->beginRenderPass( m_resources.renderPass, m_resources.framebuffer );

    // Render grid
    // Maybe I should move this to a primitive?
    // for ( float z = -100.0f; z <= 100.0f; z += 1.0f ) {
    //     DebugDrawManager::addLine( { -100.0f, 0, z }, { 100.0f, 0, z }, { 1, 1, 1 } );
    // }
    // for ( float x = -100.0f; x < 100.0f; x += 1.0f ) {
    //     DebugDrawManager::addLine( { x, 0, -100.0f }, { x, 0, 100.0f }, { 1, 1, 1 } );
    // }

    // Render selected node bounds as a box
    if ( auto editor = editor::Editor::getInstance() ) {
        if ( auto selectedNode = editor->getSelectedObject< crimild::Node >() ) {
            auto bounds = selectedNode->getWorldBound();
            const auto min = bounds->getMin();
            const auto max = bounds->getMax();

            // TODO: Create an "addBox" for debug utils
            DebugDrawManager::addLine( { min.x, min.y, min.z }, { max.x, min.y, min.z }, { 1, 1, 0 } );
            DebugDrawManager::addLine( { min.x, max.y, min.z }, { max.x, max.y, min.z }, { 1, 1, 0 } );
            DebugDrawManager::addLine( { min.x, min.y, min.z }, { min.x, max.y, min.z }, { 1, 1, 0 } );
            DebugDrawManager::addLine( { max.x, min.y, min.z }, { max.x, max.y, min.z }, { 1, 1, 0 } );

            DebugDrawManager::addLine( { min.x, min.y, max.z }, { max.x, min.y, max.z }, { 1, 1, 0 } );
            DebugDrawManager::addLine( { min.x, max.y, max.z }, { max.x, max.y, max.z }, { 1, 1, 0 } );
            DebugDrawManager::addLine( { min.x, min.y, max.z }, { min.x, max.y, max.z }, { 1, 1, 0 } );
            DebugDrawManager::addLine( { max.x, min.y, max.z }, { max.x, max.y, max.z }, { 1, 1, 0 } );

            DebugDrawManager::addLine( { min.x, min.y, min.z }, { min.x, min.y, max.z }, { 1, 1, 0 } );
            DebugDrawManager::addLine( { min.x, max.y, min.z }, { min.x, max.y, max.z }, { 1, 1, 0 } );

            DebugDrawManager::addLine( { max.x, min.y, min.z }, { max.x, min.y, max.z }, { 1, 1, 0 } );
            DebugDrawManager::addLine( { max.x, max.y, min.z }, { max.x, max.y, max.z }, { 1, 1, 0 } );
        }
    }

    // Render axis
    DebugDrawManager::addLine( { 0, 0, 0 }, { 100, 0, 0 }, { 1, 0, 0 } );
    DebugDrawManager::addLine( { 0, 0, 0 }, { 0, 100, 0 }, { 0, 1, 0 } );
    DebugDrawManager::addLine( { 0, 0, 0 }, { 0, 0, 100 }, { 0, 0, 1 } );

    auto scene = Simulation::getInstance()->getScene();
    if ( scene != nullptr && m_camera != nullptr ) {
        const auto view = m_camera->getViewMatrix();
        const auto proj = m_camera->getProjectionMatrix();

        scene->perform( DebugVisitor( m_camera.get() ) );

        DebugDrawManager::eachRenderable(
            [ & ]( auto renderable ) {
                cmds->bindPipeline( m_resources.pipeline );

                cmds->pushConstants(
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    PushConstantsData {
                        .mvp = proj * view * Matrix4( renderable.world ),
                        .color = renderable.color,
                    }
                );

                cmds->drawPrimitive( renderable.primitive );
            }
        );
    }

    DebugDrawManager::reset();

    cmds->endRenderPass();

    cmds->pipelineBarrier(
        ImageMemoryBarrier {
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = m_outputTarget->getImageView(),
        }
    );

    cmds->end( m_syncOptions );
    getRenderDevice()->submitGraphicsCommands( cmds, m_syncOptions.wait, m_syncOptions.signal );
}
