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

#include "Rendering/Operations/Operations_debugLightCascades.hpp"

#include "Components/MaterialComponent.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/FetchLights.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::debugLightCascades(
    SharedPointer< FrameGraphResource > const &albedo,
    SharedPointer< FrameGraphResource > const &positions,
    SharedPointer< FrameGraphResource > const &normals,
    SharedPointer< FrameGraphResource > const &materials,
    SharedPointer< FrameGraphResource > const &depth,
    SharedPointer< FrameGraphResource > const &shadowAtlas ) noexcept
{
    // TODO: move this to a compute pass
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "debug/ligthCascades" );

    auto color = useColorAttachment( renderPass->getName() + "/color", Format::R32G32B32A32_SFLOAT );

    renderPass->attachments = { color };

    auto createPipeline = [ & ]( auto lightType ) {
        auto pipeline = crimild::alloc< GraphicsPipeline >();
        pipeline->setProgram(
            [ & ] {
                auto program = crimild::alloc< ShaderProgram >();
                program->setShaders(
                    Array< SharedPointer< Shader > > {
                        crimild::alloc< Shader >(
                            Shader::Stage::VERTEX,
                            R"(
                                vec2 positions[6] = vec2[](
                                    vec2( -1.0, 1.0 ),
                                    vec2( -1.0, -1.0 ),
                                    vec2( 1.0, -1.0 ),

                                    vec2( -1.0, 1.0 ),
                                    vec2( 1.0, -1.0 ),
                                    vec2( 1.0, 1.0 )
                                );

                                vec2 texCoords[6] = vec2[](
                                    vec2( 0.0, 0.0 ),
                                    vec2( 0.0, 1.0 ),
                                    vec2( 1.0, 1.0 ),

                                    vec2( 0.0, 0.0 ),
                                    vec2( 1.0, 1.0 ),
                                    vec2( 1.0, 0.0 )
                                );

                                void main()
                                {
                                    gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
                                }
                            )" ),
                        crimild::alloc< Shader >(
                            Shader::Stage::FRAGMENT,
                            R"(
                                layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                    mat4 view;
                                    mat4 proj;
                                    vec2 viewport;
                                };

                                layout ( set = 0, binding = 1 ) uniform sampler2D uAlbedo;
                                layout ( set = 0, binding = 2 ) uniform sampler2D uPositions;
                                layout ( set = 0, binding = 3 ) uniform sampler2D uNormals;
                                layout ( set = 0, binding = 4 ) uniform sampler2D uMaterials;
                                layout ( set = 0, binding = 5 ) uniform sampler2D uShadowAtlas;

                                layout ( set = 1, binding = 0 ) uniform LightProps {
                                    uint type;
                                    vec4 position;
                                    vec4 direction;
                                    vec4 ambient;
                                    vec4 color;
                                    vec4 attenuation;
                                    vec4 cutoff;
                                    uint castShadows;
                                    float shadowBias;
                                    vec4 cascadeSplits;
                                    mat4 lightSpaceMatrix[ 4 ];
                                    vec4 viewport;
                                    float energy;
                                    float radius;
                                } uLight;

                                layout ( location = 0 ) out vec4 outColor;

                                void main()
                                {
                                    vec2 uv = gl_FragCoord.st / viewport;

                                    vec4 baseColor = texture( uAlbedo, uv );
                                    if ( baseColor.a == 0 ) {
                                        // nothing to render. discard and avoid complex calculations
                                        discard;
                                    }

                                    vec4 positionData = texture( uPositions, uv );

                                    // z is a negative value
                                    float z = positionData.w;

                                    vec3 lightCascadeColor = vec3( 1, 0, 0 );

                                    if ( z < uLight.cascadeSplits[ 0 ] ) {
                                       lightCascadeColor = vec3( 1, 1, 0 );
                                    }

                                    if ( z < uLight.cascadeSplits[ 1 ] ) {
                                       lightCascadeColor = vec3( 0, 1, 0 );
                                    }

                                    if ( z < uLight.cascadeSplits[ 2 ] ) {
                                       lightCascadeColor = vec3( 0, 1, 1 );
                                    }

                                    outColor = vec4( lightCascadeColor, 1 );
                                }
                            )" ),
                    } );
                if ( lightType != Light::Type::DIRECTIONAL ) {
                    program->vertexLayouts = { VertexP3::getLayout() };
                }
                program->descriptorSetLayouts = {
                    [] {
                        auto layout = crimild::alloc< DescriptorSetLayout >();
                        layout->bindings = {
                            {
                                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                .stage = Shader::Stage::ALL,
                            },
                            {
                                .descriptorType = DescriptorType::TEXTURE,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                .descriptorType = DescriptorType::TEXTURE,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                .descriptorType = DescriptorType::TEXTURE,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                .descriptorType = DescriptorType::TEXTURE,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                .descriptorType = DescriptorType::TEXTURE,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                        };
                        return layout;
                    }(),
                    [] {
                        auto layout = crimild::alloc< DescriptorSetLayout >();
                        layout->bindings = {
                            {
                                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                .stage = Shader::Stage::ALL,
                            },
                        };
                        return layout;
                    }(),
                };
                return program;
            }() );
        pipeline->depthStencilState.depthTestEnable = false;
        pipeline->colorBlendState = ColorBlendState {
            .enable = true,
            .srcColorBlendFactor = BlendFactor::ONE, //DST_COLOR,
            .dstColorBlendFactor = BlendFactor::ONE, //ZERO,
        };
        pipeline->rasterizationState = RasterizationState {
            .cullMode = lightType == Light::Type::DIRECTIONAL ? CullMode::BACK : CullMode::FRONT, // Render back faces only
        };
        return pipeline;
    };

    auto pointLightPipeline = createPipeline( Light::Type::POINT );
    auto directionalLightPipeline = createPipeline( Light::Type::DIRECTIONAL );

    auto lightVolume = crimild::alloc< SpherePrimitive >(
        SpherePrimitive::Params {
            .type = Primitive::Type::TRIANGLES,
            .layout = VertexP3::getLayout(),
            .radius = 1.0,
        } );

    auto descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = crimild::alloc< CameraViewProjectionUniform >( Camera::getMainCamera() ),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), albedo ),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), positions ),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), normals ),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), materials ),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), shadowAtlas ),
            },
        };
        return descriptorSet;
    }();

    auto viewport = ViewportDimensions {
        .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
        .dimensions = Rectf { { 0, 0 }, { 1, 1 } },
    };

    renderPass->reads(
        {
            albedo,
            positions,
            normals,
            materials,
            depth,
            shadowAtlas,
        } );
    renderPass->writes( { color } );
    renderPass->produces( { color } );

    return withDynamicGraphicsCommands(
        renderPass,
        [ lightVolume,
          pointLightPipeline,
          directionalLightPipeline,
          descriptors,
          viewport ]( auto commandBuffer ) {
            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );

            FetchLights fetchLights;
            auto scene = Simulation::getInstance()->getScene();
            scene->perform( fetchLights );
            fetchLights.forEachLight(
                [ & ]( auto light ) {
                    if ( light->getType() == Light::Type::DIRECTIONAL ) {
                        commandBuffer->bindGraphicsPipeline( crimild::get_ptr( directionalLightPipeline ) );
                        commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
                        commandBuffer->bindDescriptorSet( light->getDescriptors() );
                        commandBuffer->draw( 6 );
                    }
                } );
        } );
}
