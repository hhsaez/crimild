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
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/FetchLights.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::iblPass(
    SharedPointer< FrameGraphResource > const &albedo,
    SharedPointer< FrameGraphResource > const &positions,
    SharedPointer< FrameGraphResource > const &normals,
    SharedPointer< FrameGraphResource > const &materials,
    SharedPointer< FrameGraphResource > const &depth,
    SharedPointer< FrameGraphResource > const &reflectionAtlas,
    SharedPointer< FrameGraphResource > const &irradianceAtlas,
    SharedPointer< FrameGraphResource > const &prefilterAtlas,
    SharedPointer< FrameGraphResource > const &brdfLUT ) noexcept
{
    // TODO: move this to a compute pass
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "iblPass" );

    auto color = useColorAttachment( "ibl/color", Format::R32G32B32A32_SFLOAT );

    renderPass->attachments = { color };

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

                                layout ( location = 0 ) out vec2 outTexCoord;

                                void main()
                                {
                                    gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
                                    outTexCoord = texCoords[ gl_VertexIndex ];
                                }
                            )" ),
                        crimild::alloc< Shader >(
                            Shader::Stage::FRAGMENT,
                            R"(
                                #include <textureCube>

                                layout ( location = 0 ) in vec2 inTexCoord;

                                layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                    mat4 view;
                                    mat4 proj;
                                    vec2 viewport;
                                };

                                layout ( set = 0, binding = 1 ) uniform sampler2D uAlbedo;
                                layout ( set = 0, binding = 2 ) uniform sampler2D uPositions;
                                layout ( set = 0, binding = 3 ) uniform sampler2D uNormals;
                                layout ( set = 0, binding = 4 ) uniform sampler2D uMaterials;
                                layout ( set = 0, binding = 5 ) uniform sampler2D uReflectionAtlas;
                                layout ( set = 0, binding = 6 ) uniform sampler2D uIrradianceAtlas;
                                layout ( set = 0, binding = 7 ) uniform sampler2D uPrefilterAtlas;
                                layout ( set = 0, binding = 8 ) uniform sampler2D uBRDFLUT;

                                layout ( location = 0 ) out vec4 outColor;

                                const float PI = 3.14159265359;

                                float distributionGGX( vec3 N, vec3 H, float roughness )
                                {
                                    float a = roughness * roughness;
                                    float a2 = a * a;
                                    float NdotH = max( dot( N, H ), 0.0 );
                                    float NdotH2 = NdotH * NdotH;

                                    float num = a2;
                                    float denom = ( NdotH2 * ( a2 - 1.0 ) + 1.0 );
                                    denom = PI * denom * denom;

                                    return num / denom;
                                }

                                float geometrySchlickGGX( float NdotV, float roughness )
                                {
                                    float r = ( roughness + 1.0 );
                                    float k = ( r * r ) / 8.0;
                                    float num = NdotV;
                                    float denom = NdotV * ( 1.0 - k ) + k;
                                    return num / denom;
                                }

                                float geometrySmith( vec3 N, vec3 V, vec3 L, float roughness )
                                {
                                    float NdotV = max( dot( N, V ), 0.0 );
                                    float NdotL = max( dot( N, L ), 0.0 );
                                    float ggx1 = geometrySchlickGGX( NdotL, roughness );
                                    float ggx2 = geometrySchlickGGX( NdotV, roughness );
                                    return ggx1 * ggx2;
                                }

                                vec3 fresnelSchlick( float cosTheta, vec3 F0 )
                                {
                                    return F0 + ( 1.0 - F0 ) * pow( 1.0 - cosTheta, 5.0 );
                                }

                                vec3 fresnelSchlickRoughness( float cosTheta, vec3 F0, float roughness )
                                {
                                    return F0 + ( max( vec3( 1.0 - roughness ), F0 ) - F0 ) * pow( 1.0 - cosTheta, 5.0 );
                                }

                                vec3 computeF0( vec3 albedo, float metallic )
                                {
                                    vec3 F0 = vec3( 0.04 );
                                    F0 = mix( F0, albedo, metallic );
                                    return F0;
                                }

                                vec3 brdf( vec3 N, vec3 H, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness )
                                {
                                    vec3 F0 = computeF0( albedo, metallic );

                                    float NDF = distributionGGX( N, H, roughness );
                                    float G = geometrySmith( N, V, L, roughness );
                                    vec3 F = fresnelSchlick( max( dot( H, V ), 0.0 ), F0 );

                                    vec3 kS = F;
                                    vec3 kD = vec3( 1.0 ) - kS;
                                    kD *= 1.0 - metallic;

                                    vec3 numerator = NDF * G * F;
                                    float denominator = 4.0 * max( dot( N, V ), 0.0 ) * max( dot( N, L ), 0.0 );
                                    vec3 specular = numerator / max( denominator, 0.001 );

                                    // add outgoing radiance
                                    float NdotL = max( dot( N, L ), 0.0 );
                                    return ( kD * albedo / PI + specular ) * radiance * NdotL;
                                }

                                int roughnessToLOD( float roughness, float maxLOD )
                                {
                                    return int( round( roughness * maxLOD ) );
                                }

                                vec3 getPrefilteredColor( vec3 N, vec3 V, float roughness )
                                {
                                    const float MAX_REFLECTION_LOD = 4.0;
                                    const int lod = roughnessToLOD( roughness, MAX_REFLECTION_LOD );
                                    const vec2 texelSize = 1.0 / textureSize( uPrefilterAtlas, lod );
                                    const vec3 R = reflect( -V, N );

                                    vec3 forward = R;
                                    vec3 up = V;
                                    vec3 right = normalize( cross( up, forward ) );
                                    up = normalize( cross( right, forward ) );

                                    // compute a simple bilinear color by sampling using different vectors
                                    // this is not ideal, but it helps reduce some visual artifacts
                                    vec3 color = textureCubeLOD( uPrefilterAtlas, R, lod ).rgb;
                                    color += textureCubeLOD( uPrefilterAtlas, normalize( R + texelSize.x * up + 0.0 * right ), lod ).rgb;
                                    color += textureCubeLOD( uPrefilterAtlas, normalize( R + texelSize.x * up + texelSize.y * right ), lod ).rgb;
                                    color += textureCubeLOD( uPrefilterAtlas, normalize( R + 0.0 * up + 0.0 * right ), lod ).rgb;
                                    color += textureCubeLOD( uPrefilterAtlas, normalize( R + 0.0 * up + texelSize.y * right ), lod ).rgb;
                                    color /= 5.0;

                                    return color;
                                }

                                vec3 irradiance( vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, float ao )
                                {
                                    vec3 F0 = computeF0( albedo, metallic );
                                    vec3 F = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );

                                    vec3 kS = F;
                                    vec3 kD = 1.0 - kS;
                                    kD *= 1.0 - metallic;

                                    vec3 I = textureCubeUV( uIrradianceAtlas, N, vec4( 0, 0, 1, 1 ) ).rgb;
                                    vec3 diffuse = I * albedo;

                                    const float MAX_REFLECTION_LOD = 4.0;

                                    vec3 prefilteredColor = getPrefilteredColor( N, V, roughness );
                                    vec2 envBRDF = texture( uBRDFLUT, vec2( max( dot( N, V ), 0.0 ), roughness ) ).rg;
                                    vec3 specular = prefilteredColor * ( F * envBRDF.x + envBRDF.y );

                                    return ( kD * diffuse + specular ) * ao;
                                }

                                void main()
                                {
                                    vec2 uv = gl_FragCoord.st / viewport;

                                    vec4 baseColor = texture( uAlbedo, uv );
                                    if ( baseColor.a == 0 ) {
                                        // nothing to render. discard and avoid complex calculations
                                        discard;
                                    }

                                    vec3 albedo = baseColor.rgb;
                                    vec3 P = texture( uPositions, uv ).xyz;
                                    vec3 N = texture( uNormals, uv ).xyz;
                                    vec4 material = texture( uMaterials, uv );
                                    float metallic = material.x;
                                    float roughness = material.y;
                                    float ambientOcclusion = material.z;

                                    vec3 E = inverse( view )[ 3 ].xyz;
                                    vec3 V = normalize( E - P );

                                    vec3 ambient = irradiance( N, V, albedo, metallic, roughness, ambientOcclusion );

                                    outColor = vec4( ambient, 1.0 );
                                }
                            )" ),
                    } );
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
        return pipeline;
    }();

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
                .obj = withResource( crimild::alloc< Texture >(), reflectionAtlas ),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), irradianceAtlas ),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), prefilterAtlas ),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), brdfLUT ),
            },
        };
        return descriptorSet;
    }();

    auto viewport = ViewportDimensions {
        .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
        .dimensions = Rectf { { 0, 0 }, { 1, 1 } },
    };

    renderPass->reads(
        { albedo,
          positions,
          normals,
          materials,
          depth,
          reflectionAtlas,
          irradianceAtlas,
          prefilterAtlas,
          brdfLUT } );
    renderPass->writes( { color } );
    renderPass->produces( { color } );

    return withDynamicGraphicsCommands(
        renderPass,
        [ pipeline, descriptors, viewport ]( auto commandBuffer ) {
            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
            commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
            commandBuffer->draw( 6 );
        } );
}
