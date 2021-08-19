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
#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::gBufferPass( SharedPointer< FrameGraphResource > const renderables ) noexcept
{
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "gBufferPass" );

    auto albedo = useColorAttachment( "gBuffer/albedo" );
    auto position = useColorAttachment( "gBuffer/position", Format::R32G32B32A32_SFLOAT );
    auto normal = useColorAttachment( "gBuffer/normal", Format::R32G32B32A32_SFLOAT );
    auto material = useColorAttachment( "gBuffer/material", Format::R32G32B32A32_SFLOAT );
    auto depth = useDepthAttachment( "gBuffer/depth" );

    renderPass->attachments = { albedo, position, normal, material, depth };

    auto descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = crimild::alloc< CameraViewProjectionUniform >( Camera::getMainCamera() ),
            },
        };
        return descriptorSet;
    }();

    // TODO: move this to a material
    auto pipeline = [ & ] {
        auto pipeline = crimild::alloc< GraphicsPipeline >();
        pipeline->setProgram(
            [ & ] {
                auto program = crimild::alloc< ShaderProgram >();
                program->setShaders(
                    Array< SharedPointer< Shader > > {
                        crimild::alloc< Shader >(
                            Shader::Stage::VERTEX,
                            R"(
                                layout( location = 0 ) in vec3 inPosition;
                                layout( location = 1 ) in vec3 inNormal;
                                layout( location = 2 ) in vec2 inTexCoord;

                                layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                    mat4 view;
                                    mat4 proj;
                                };

                                layout( set = 2, binding = 0 ) uniform GeometryUniforms {
                                    mat4 model;
                                };

                                layout( location = 0 ) out vec3 outNormal;
                                layout( location = 1 ) out vec3 outPosition;
                                layout( location = 2 ) out vec3 outViewPosition;
                                layout( location = 3 ) out vec2 outTexCoord;
                                layout( location = 4 ) out vec3 outEyePosition;
                                layout( location = 5 ) out mat3 outViewMatrix3;

                                void main() {
                                    gl_Position = proj * view * model * vec4( inPosition, 1.0 );

                                    mat3 nMatrix = mat3( transpose( inverse( model ) ) );

                                    outViewMatrix3 = mat3( inverse( view ) );
                                    outNormal = normalize( nMatrix * inNormal );
                                    outPosition = ( model * vec4( inPosition, 1.0 ) ).xyz;
                                    outViewPosition = ( view * model * vec4( inPosition, 1.0 ) ).xyz;
                                    outEyePosition = inverse( view )[ 3 ].xyz;
                                    outTexCoord = inTexCoord;
                                }
                            )" ),
                        crimild::alloc< Shader >(
                            Shader::Stage::FRAGMENT,
                            R"(
                                layout( location = 0 ) in vec3 inNormal;
                                layout( location = 1 ) in vec3 inPosition;
                                layout( location = 2 ) in vec3 inViewPosition;
                                layout( location = 3 ) in vec2 inTexCoord;
                                layout( location = 4 ) in vec3 inEyePosition;
                                layout( location = 5 ) in mat3 inViewMatrix3;

                                layout( set = 1, binding = 0 ) uniform Material
                                {
                                    vec3 albedo;
                                    float metallic;
                                    float roughness;
                                    float ambientOcclusion;
                                } uMaterial;

                                layout( set = 1, binding = 1 ) uniform sampler2D uAlbedoMap;
                                layout( set = 1, binding = 2 ) uniform sampler2D uMetallicMap;
                                layout( set = 1, binding = 3 ) uniform sampler2D uRoughnessMap;
                                layout( set = 1, binding = 4 ) uniform sampler2D uAmbientOcclusionMap;
                                layout( set = 1, binding = 5 ) uniform sampler2D uCombinedRoughnessMetallicMap;
                                layout( set = 1, binding = 6 ) uniform sampler2D uNormalMap;

                                layout ( location = 0 ) out vec4 outAlbedo;
                                layout ( location = 1 ) out vec4 outPosition;
                                layout ( location = 2 ) out vec4 outNormal;
                                layout ( location = 3 ) out vec4 outMaterial;

                                // Computes T, B and TBN matrix based on surface N
                                // TODO (hernan): is this in view space? If so, we transform T and B to world space
                                mat3 computeTBN( mat3 invView, vec3 N )
                                {
                                    vec3 q1 = dFdx( inPosition );
                                    vec3 q2 = dFdy( inPosition );
                                    vec2 st1 = dFdx( inTexCoord );
                                    vec2 st2 = dFdy( inTexCoord );
                                    vec3 T = normalize( invView * ( q1 * st2.t - q2 * st1.t ) );
                                    vec3 B = -normalize( invView * ( -q1 * st2.s + q2 * st1.s ) );
                                    return mat3( T, B, N );
                                }

                                void main()
                                {
                                    // Load a normal from the Normal map.
                                    vec3 N = texture( uNormalMap, inTexCoord ).rgb;
                                    if ( N.x * N.y * N.z != 0 ) {
                                        // Transform normal to range [-1, 1]
                                        N = normalize( 2.0 * N - 1.0 );
                                        N = normalize( computeTBN( inViewMatrix3, inNormal ) * N );
                                    } else {
                                        // If no normal map is present, the normal vector will be zero
                                        // In this case, use the input normal from the vertex shader
                                        N = normalize( inNormal );
                                    }

                                    vec3 P = inPosition;

                                    vec4 albedoMapColor = texture( uAlbedoMap, inTexCoord );
                                    if ( albedoMapColor.a < 0.01 ) {
                                        discard;
                                    }

                                    vec3 albedo = uMaterial.albedo.rgb * pow( albedoMapColor.rgb, vec3( 2.2 ) );
                                    float metallic = uMaterial.metallic * texture( uMetallicMap, inTexCoord ).r;
                                    float roughness = uMaterial.roughness * texture( uRoughnessMap, inTexCoord ).r;
                                    float ambientOcclusion = uMaterial.ambientOcclusion * texture( uAmbientOcclusionMap, inTexCoord ).r;

                                    vec4 combinedPBR = texture( uCombinedRoughnessMetallicMap, inTexCoord );
                                    roughness *= combinedPBR.g;
                                    metallic *= combinedPBR.b;

                                    metallic = clamp( metallic, 0.0, 1.0 );
	                                roughness = clamp( roughness, 0.05, 0.999 );

                                    outAlbedo = vec4( albedo, 1.0 );

                                    //outPosition = vec4( P, inViewPosition.z );
                                    outPosition = vec4( P, gl_FragCoord.z );
                                    outNormal = vec4( N, 1.0 );
                                    outMaterial = vec4( metallic, roughness, ambientOcclusion, 1.0 );
                                }
                            )" ),
                    } );
                program->vertexLayouts = { VertexP3N3TC2::getLayout() };
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
                                // Material properties
                                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                // Albedo Map
                                .descriptorType = DescriptorType::ALBEDO_MAP,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                // Metallic Map
                                .descriptorType = DescriptorType::METALLIC_MAP,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                // Roughness Map
                                .descriptorType = DescriptorType::ROUGHNESS_MAP,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                // Ambient Occlusion Map
                                .descriptorType = DescriptorType::AMBIENT_OCCLUSION_MAP,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                // Ambient Occlusion Map
                                .descriptorType = DescriptorType::COMBINED_ROUGHNESS_METALLIC_MAP,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                // Normal Map
                                .descriptorType = DescriptorType::NORMAL_MAP,
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
                                .stage = Shader::Stage::VERTEX,
                            },
                        };
                        return layout;
                    }(),
                };
                return program;
            }() );
        return pipeline;
    }();

    auto viewport = ViewportDimensions {
        .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
        .dimensions = Rectf { { 0, 0 }, { 1, 1 } },
    };

    renderPass->reads( { renderables } );
    renderPass->writes( { albedo, position, normal, material, depth } );
    renderPass->produces( { albedo, position, normal, material, depth } );

    return withDynamicGraphicsCommands(
        renderPass,
        [ pipeline,
          descriptors,
          viewport,
          renderables = crimild::cast_ptr< RenderableSet >( renderables ) ]( auto commandBuffer ) {
            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            renderables->eachGeometry(
                [ & ]( Geometry *geometry ) {
                    if ( auto ms = geometry->getComponent< MaterialComponent >() ) {
                        if ( auto material = ms->first() ) {
                            commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) ); // material->getGraphicsPipeline() );
                            commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
                            commandBuffer->bindDescriptorSet( material->getDescriptors() );
                            commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                            commandBuffer->drawPrimitive( geometry->anyPrimitive() );
                        }
                    }
                } );
        } );
}
