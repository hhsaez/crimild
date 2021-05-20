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

#include "Components/MaterialComponent.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/Vertex.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Light.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/FetchLights.hpp"

using namespace crimild;

static SharedPointer< GraphicsPipeline > createPipeline( Light::Type lightType ) noexcept
{
    auto pipeline = crimild::alloc< GraphicsPipeline >();
    pipeline->setProgram(
        [ & ] {
            auto program = crimild::alloc< ShaderProgram >(
                Array< SharedPointer< Shader > > {
                    crimild::alloc< Shader >(
                        Shader::Stage::VERTEX,
                        R"(
                        layout ( location = 0 ) in vec3 inPosition;
                        layout ( location = 1 ) in vec3 inNormal;
                        layout ( location = 2 ) in vec2 inTextureCoord;

                        layout ( set = 0, binding = 0 ) uniform LightUniforms {
                            //mat4 lightSpaceMatrix;
                            mat4 proj;
                            mat4 view;
                            vec3 lightPos;
                        };

                        layout ( set = 1, binding = 0 ) uniform GeometryUniforms {
                            mat4 model;
                        };

                        layout ( location = 0 ) out vec3 outPosition;
                        layout ( location = 1 ) out vec3 outLightPos;

                        void main()
                        {
                            gl_Position = proj * view * model * vec4( inPosition, 1.0 );

                            outPosition = ( model * vec4( inPosition, 1.0 ) ).xyz;
                            outLightPos = lightPos;
                        }
                    )" ),
                    crimild::alloc< Shader >(
                        Shader::Stage::FRAGMENT,
                        lightType == Light::Type::POINT
                            ? R"(
                            layout ( location = 0 ) in vec3 inPosition;
                            layout ( location = 1 ) in vec3 inLightPos;

                            layout ( location = 0 ) out float fragColor;

                            void main()
                            {
                                float d = length( inPosition - inLightPos );
                                fragColor = d;
                            }
                        )"
                            : R"(
                            layout ( location = 0 ) out float fragColor;

                            void main()
                            {
                                float d = gl_FragCoord.z;
                                fragColor = d;
                            }
                        )" ),
                } );
            program->vertexLayouts = { VertexLayout::P3_N3_TC2 };
            program->descriptorSetLayouts = {
                [] {
                    auto layout = crimild::alloc< DescriptorSetLayout >();
                    layout->bindings = {
                        {
                            .descriptorType = DescriptorType::UNIFORM_BUFFER,
                            .stage = Shader::Stage::VERTEX,
                        },
                    };
                    return layout;
                }(),
                [] {
                    auto layout = crimild::alloc< DescriptorSetLayout >();
                    layout->bindings = {
                        {
                            .descriptorType = DescriptorType::UNIFORM_BUFFER,
                            .stage = Shader::Stage::VERTEX,
                        },
                    };
                    return layout;
                }(),
            };
            return program;
        }() );
    pipeline->viewport = { .scalingMode = ScalingMode::DYNAMIC };
    pipeline->scissor = { .scalingMode = ScalingMode::DYNAMIC };
    pipeline->rasterizationState = RasterizationState {
        .cullMode = CullMode::FRONT_AND_BACK,

        // It would be ideal to support this feature for directional lights
        // but it seems it doesn't work...
        //.depthClampEnable = true,
    };

    return pipeline;
}

struct LightUniforms {
    alignas( 16 ) Matrix4f proj;
    alignas( 16 ) Matrix4f view;
    alignas( 16 ) Vector3f lightPos;
};

size_t recordPointLightCommands(
    CommandBuffer *commandBuffer,
    SharedPointer< GraphicsPipeline > pipeline,
    Array< ViewportDimensions > &layout,
    size_t offset,
    Array< Light * > lights,
    SharedPointer< RenderableSet > renderables ) noexcept
{
    lights.each(
        [ & ]( auto light ) {
            if ( offset >= layout.size() ) {
                CRIMILD_LOG_WARNING( "No available viewport in layout in shadow atlas for spot light" );
                return;
            }

            auto layoutViewport = layout[ offset++ ];

            light->getShadowMap()->setViewport(
                [ & ] {
                    auto rect = layoutViewport.dimensions;
                    return Vector4f(
                        rect.getX(),
                        rect.getY(),
                        rect.getWidth(),
                        rect.getHeight() );
                }() );

            auto transformViewport = []( auto layout, auto viewport ) {
                auto ld = layout.dimensions;
                auto vd = viewport.dimensions;
                return ViewportDimensions {
                    .scalingMode = ScalingMode::RELATIVE,
                    .dimensions = Rectf(
                        ld.getX() + vd.getX() * ld.getWidth(),
                        ld.getY() + vd.getY() * ld.getHeight(),
                        ld.getWidth() * vd.getWidth(),
                        ld.getHeight() * vd.getHeight() ),
                };
            };

            ViewportDimensions viewports[ 6 ] = {
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.0f, 0.5f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.5f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.25f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.75f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.25f, 0.5f, 0.25f, 0.25f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.75f, 0.5f, 0.25f, 0.25f ),
                    } ),
            };

            for ( auto face = 0l; face < 6; ++face ) {
                // render the scene for each cubemap face
                auto viewport = viewports[ face ];
                commandBuffer->setViewport( viewport );
                commandBuffer->setScissor( viewport );
                renderables->eachGeometry(
                    [ & ]( Geometry *geometry ) {
                        if ( geometry->getLayer() == Node::Layer::SKYBOX ) {
                            // ignore skybox
                            return;
                        }
                        if ( auto ms = geometry->getComponent< MaterialComponent >() ) {
                            if ( auto m = ms->first() ) {
                                if ( !m->castShadows() ) {
                                    return;
                                }
                            }
                        }
                        // TODO: binding pipeline and render pass descriptors should be done earlier, right?
                        commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
                        commandBuffer->bindDescriptorSet( crimild::get_ptr( light->getShadowAtlasDescriptors()[ face ] ) );
                        commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                        commandBuffer->drawPrimitive( geometry->anyPrimitive() );
                    } );
            }
        } );

    return offset;
}

size_t recordSpotLightCommands(
    CommandBuffer *commandBuffer,
    SharedPointer< GraphicsPipeline > pipeline,
    Array< ViewportDimensions > &layout,
    size_t offset,
    Array< Light * > lights,
    SharedPointer< RenderableSet > renderables ) noexcept
{
    lights.each(
        [ & ]( auto light ) {
            if ( offset >= layout.size() ) {
                CRIMILD_LOG_WARNING( "No available viewport in layout in shadow atlas for spot light" );
                return;
            }

            auto viewport = layout[ offset++ ];

            light->getShadowMap()->setViewport(
                [ & ] {
                    auto rect = viewport.dimensions;
                    return Vector4f(
                        rect.getX(),
                        rect.getY(),
                        rect.getWidth(),
                        rect.getHeight() );
                }() );

            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            renderables->eachGeometry(
                [ & ]( Geometry *geometry ) {
                    commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
                    commandBuffer->bindDescriptorSet( crimild::get_ptr( light->getShadowAtlasDescriptors()[ 0 ] ) );
                    commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                    commandBuffer->drawPrimitive( geometry->anyPrimitive() );
                } );
        } );

    return offset;
}

size_t recordDirectionalLightCommands(
    CommandBuffer *commandBuffer,
    SharedPointer< GraphicsPipeline > pipeline,
    Array< ViewportDimensions > &layout,
    size_t offset,
    Array< Light * > lights,
    SharedPointer< RenderableSet > renderables ) noexcept
{
    // TODO: move this to ViewportDimensions
    static auto transformViewport = []( auto layout, auto viewport ) {
        auto ld = layout.dimensions;
        auto vd = viewport.dimensions;
        return ViewportDimensions {
            .scalingMode = ScalingMode::RELATIVE,
            .dimensions = Rectf(
                ld.getX() + vd.getX() * ld.getWidth(),
                ld.getY() + vd.getY() * ld.getHeight(),
                ld.getWidth() * vd.getWidth(),
                ld.getHeight() * vd.getHeight() ),
        };
    };

    auto recordCascadeCommands = [ & ]( auto light, auto cascadeId, auto viewport ) {
        commandBuffer->setViewport( viewport );
        commandBuffer->setScissor( viewport );
        renderables->eachGeometry(
            [ & ]( Geometry *geometry ) {
                commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
                commandBuffer->bindDescriptorSet( crimild::get_ptr( light->getShadowAtlasDescriptors()[ cascadeId ] ) );
                commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                commandBuffer->drawPrimitive( geometry->anyPrimitive() );
            } );
    };

    lights.each(
        [ & ]( auto light ) {
            if ( offset >= layout.size() ) {
                CRIMILD_LOG_WARNING( "No available viewport in layout in shadow atlas for spot light" );
                return;
            }

            auto layoutViewport = layout[ offset++ ];

            // Assign a single viewport to the shadow map. Since each cascade viewport is hard-coded to
            // take only a quarter of the available region, we can use that information to compute the
            // actual viewport in the shader without having to pass this information explicitly.
            light->getShadowMap()->setViewport(
                [ & ] {
                    auto rect = layoutViewport.dimensions;
                    return Vector4f(
                        rect.getX(),
                        rect.getY(),
                        rect.getWidth(),
                        rect.getHeight() );
                }() );

            ViewportDimensions cascadeViewports[ 4 ] = {
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.0f, 0.0f, 0.5f, 0.5f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.0f, 0.5f, 0.5f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.0f, 0.5f, 0.5f, 0.5f ),
                    } ),
                transformViewport(
                    layoutViewport,
                    ViewportDimensions {
                        .scalingMode = ScalingMode::RELATIVE,
                        .dimensions = Rectf( 0.5f, 0.5f, 0.5f, 0.5f ),
                    } ),
            };

            for ( auto i = 0l; i < 4; ++i ) {
                recordCascadeCommands(
                    light,
                    i,
                    cascadeViewports[ i ] );
            }
        } );

    return offset;
}

SharedPointer< FrameGraphOperation > crimild::framegraph::renderShadowAtlas( SharedPointer< FrameGraphResource > const renderables ) noexcept
{
    auto pipelines = Map< Light::Type, SharedPointer< GraphicsPipeline > > {
        { Light::Type::DIRECTIONAL, createPipeline( Light::Type::DIRECTIONAL ) },
        { Light::Type::SPOT, createPipeline( Light::Type::SPOT ) },
        { Light::Type::POINT, createPipeline( Light::Type::POINT ) },
    };

    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "renderShadowAtlas" );

    auto color = useColorAttachment( renderPass->getName() + "/color", Format::R32_SFLOAT );
    auto depth = useDepthAttachment( renderPass->getName() + "/depth" );

    renderPass->attachments = { color, depth };

    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 4096.0f,
        .height = 4096.0f,
    };

    renderPass->clearValue = {
        .color = ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f ),
    };

    renderPass->reads( { renderables } );
    renderPass->writes( { color, depth } );
    renderPass->produces( { color, depth } );

    return withDynamicGraphicsCommands(
        renderPass,
        [ pipelines,
          renderables = crimild::cast_ptr< RenderableSet >( renderables ) ]( auto commandBuffer ) {
            FetchLights fetch;
            auto scene = Simulation::getInstance()->getScene();
            scene->perform( fetch );
            Map< Light::Type, Array< Light * > > lights;
            Size lightCount = 0;
            fetch.forEachLight(
                [ & ]( auto l ) {
                    if ( l->castShadows() ) {
                        lights[ l->getType() ].add( l );
                        ++lightCount;
                    }
                } );

            if ( lightCount == 0 ) {
                // no lights casting shadows
                return;
            }

            auto viewportLayout =
                lightCount == 1
                    ? Array< ViewportDimensions > {
                          {
                              .scalingMode = ScalingMode::RELATIVE,
                              .dimensions = Rectf( 0.0f, 0.0f, 1.0f, 1.0f ),
                          },
                      }
                    : Array< ViewportDimensions > {
                          {
                              .scalingMode = ScalingMode::RELATIVE,
                              .dimensions = Rectf( 0.0f, 0.0f, 0.5f, 0.5f ),
                          },
                          {
                              .scalingMode = ScalingMode::RELATIVE,
                              .dimensions = Rectf( 0.0f, 0.5f, 0.5f, 0.5f ),
                          },
                          {
                              .scalingMode = ScalingMode::RELATIVE,
                              .dimensions = Rectf( 0.5f, 0.0f, 0.5f, 0.5f ),
                          },
                          {
                              .scalingMode = ScalingMode::RELATIVE,
                              .dimensions = Rectf( 0.5f, 0.5f, 0.5f, 0.5f ),
                          },
                      };

            auto offset = 0l;

            offset = recordDirectionalLightCommands(
                crimild::get_ptr( commandBuffer ),
                pipelines[ Light::Type::DIRECTIONAL ],
                viewportLayout,
                offset,
                lights[ Light::Type::DIRECTIONAL ],
                renderables );

            offset = recordSpotLightCommands(
                crimild::get_ptr( commandBuffer ),
                pipelines[ Light::Type::SPOT ],
                viewportLayout,
                offset,
                lights[ Light::Type::SPOT ],
                renderables );

            offset = recordPointLightCommands(
                crimild::get_ptr( commandBuffer ),
                pipelines[ Light::Type::POINT ],
                viewportLayout,
                offset,
                lights[ Light::Type::POINT ],
                renderables );
        } );
}
