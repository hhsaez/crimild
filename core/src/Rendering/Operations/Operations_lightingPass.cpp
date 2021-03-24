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

SharedPointer< FrameGraphOperation > crimild::framegraph::lightingPass(
    SharedPointer< FrameGraphResource > const &albedo,
    SharedPointer< FrameGraphResource > const &positions,
    SharedPointer< FrameGraphResource > const &normals,
    SharedPointer< FrameGraphResource > const &materials,
    SharedPointer< FrameGraphResource > const &depth,
    SharedPointer< FrameGraphResource > const &shadowAtlas ) noexcept
{
    // TODO: move this to a compute pass
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "lightingPass" );

    auto color = useColorAttachment( "lighting/color", Format::R32G32B32A32_SFLOAT );

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
                            lightType == Light::Type::DIRECTIONAL
                                ? R"(
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
                                  )"
                                : R"(
                                layout ( location = 0 ) in vec3 inPosition;

                                layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                    mat4 view;
                                    mat4 proj;
                                };

                                layout ( set = 1, binding = 0 ) uniform LightProps {
                                    uint type;
                                    vec4 position;
                                    vec4 direction;
                                    vec4 ambient;
                                    vec4 color;
                                    vec4 attenuation;
                                    vec4 cutoff;
                                    bool castShadows;
                                    float shadowBias;
                                    vec4 cascadeSplitsd;
                                    mat4 lightSpaceMatrix[ 4 ];
                                    vec4 viewport;
                                    float energy;
                                    float radius;
                                } uLight;

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
                                    gl_Position = proj * view * vec4( ( uLight.position.xyz + uLight.radius * inPosition ), 1.0 );
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

                                const float PI = 3.14159265359;

                                const mat4 bias = mat4(
                                    0.5, 0.0, 0.0, 0.0,
                                    0.0, 0.5, 0.0, 0.0,
                                    0.0, 0.0, 1.0, 0.0,
                                    0.5, 0.5, 0.0, 1.0
                                );

                                const float FACE_INVALID = -1.0;
                                const float FACE_LEFT = 0.0;
                                const float FACE_RIGHT = 1.0;
                                const float FACE_FRONT = 2.0;
                                const float FACE_BACK = 3.0;
                                const float FACE_UP = 4.0;
                                const float FACE_DOWN = 5.0;

                                // Return the face in cubemap based on the principle component of the direction
                                float getFace( vec3 direction )
                                {
                                    vec3 absDirection = abs( direction );
                                    float face = -1.0;
                                    if ( absDirection.x > absDirection.z ) {
                                        if ( absDirection.x > absDirection.y ) {
                                            return direction.x > 0.0 ? FACE_RIGHT : FACE_LEFT;
                                        } else {
                                            return direction.y > 0.0 ? FACE_UP : FACE_DOWN;
                                        }
                                    } else {
                                        if ( absDirection.z > absDirection.y ) {
                                            return direction.z > 0.0 ? FACE_FRONT : FACE_BACK;
                                        } else {
                                            return direction.y > 0.0 ? FACE_UP : FACE_DOWN;
                                        }
                                    }
                                    return FACE_INVALID;
                                }

                                vec2 getUV( vec3 direction, float face )
                                {
                                    vec2 uv;
                                    if ( face == FACE_LEFT ) {
                                        uv = vec2( -direction.z, direction.y ) / abs( direction.x );
                                    } else if ( face == FACE_RIGHT ) {
                                        uv = vec2( direction.z, direction.y ) / abs( direction.x );
                                    } else if ( face == FACE_FRONT ) {
                                        uv = vec2( -direction.x, direction.y ) / abs( direction.z );
                                    } else if ( face == FACE_BACK ) {
                                        uv = vec2( direction.x, direction.y ) / abs( direction.z );
                                    } else if ( face == FACE_UP ) {
                                        uv = vec2( direction.x, direction.z ) / abs( direction.y );
                                    } else if ( face == FACE_DOWN ) {
                                        uv = vec2( direction.x, -direction.z ) / abs( direction.y );
                                    }
                                    return 0.5 + 0.5 * uv;
                                }

                                vec2 getFaceOffsets( float face )
                                {
                                    if ( face == FACE_LEFT ) {
                                        return vec2( 0.0, 0.5 );
                                    } else if ( face == FACE_RIGHT ) {
                                        return vec2( 0.5, 0.5 );
                                    } else if ( face == FACE_FRONT ) {
                                        return vec2( 0.25, 0.5 );
                                    } else if ( face == FACE_BACK ) {
                                        return vec2( 0.75, 0.5 );
                                    } else if ( face == FACE_UP ) {
                                        return vec2( 0.5, 0.25 );
                                    } else if ( face == FACE_DOWN ) {
                                        return vec2( 0.5, 0.75 );
                                    }
                                }

                                // Performs bilinear filtering
                                vec4 textureCubeUV( sampler2D envMap, vec3 direction, vec4 viewport )
                                {
                                    const float faceSize = 0.25;
                                    const vec2 texelSize = 1.0 / textureSize( envMap, 0 );

                                    float face = getFace( direction );
                                    vec2 faceOffsets = getFaceOffsets( face );

                                    vec2 uv = getUV( direction, face );
                                    uv.y = 1.0 - uv.y;
                                    uv = faceOffsets + faceSize * uv;
                                    uv = viewport.xy + uv * viewport.zw;

                                    // make sure UV values are within the face to avoid most artifacts in the borders
                                    // of the cube map. Some visual artifacts migth still appear, though, but they
                                    // should be rare.
                                    vec2 fBL = viewport.xy + ( faceOffsets ) * viewport.zw;
                                    vec2 fTR = viewport.xy + ( faceOffsets + vec2( faceSize ) ) * viewport.zw;
                                    uv = clamp( uv, fBL + texelSize, fTR - 2.0 * texelSize );

                                    vec4 color = texture( envMap, uv );
                                    return color;
                                }

                                vec4 textureCubeLOD( sampler2D envMap, vec3 D, int lod )
                                {
                                    vec4 viewport = vec4( 0, 0, 1, 1 );
                                    if ( lod == 0 ) {
                                        viewport = vec4( 0, 0, 0.6666666667, 1.0 );
                                    } else if ( lod == 1 ) {
                                        viewport = vec4( 0.6666666667, 0, 0.3333333333, 0.3333333333 );
                                    } else if ( lod == 2 ) {
                                        viewport = vec4( 0.6666666667, 0.3333333333, 0.1666666667, 0.1666666667 );
                                    } else if ( lod == 3 ) {
                                        viewport = vec4( 0.6666666667, 0.5, 0.08333333333, 0.08333333333 );
                                    } else {
                                        viewport = vec4( 0.6666666667, 0.5833333333, 0.04166666667, 0.04166666667 );
                                    }
                                    return textureCubeUV(
                                        envMap,
                                        D,
                                        viewport
                                    );
                                }

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

float calculateShadow( vec4 ligthSpacePosition, vec4 viewport, vec3 N, vec3 L, float bias )
{
    // perform perspective divide
    vec3 projCoords = ligthSpacePosition.xyz / ligthSpacePosition.w;
    projCoords.t = 1.0 - projCoords.t;

    // Compute shadow PCF
    float depth = projCoords.z;
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize( uShadowAtlas, 0 );
    for ( int y = -1; y <= 1; ++y ) {
        for ( int x = -1; x <= 1; ++x ) {
            vec2 uv = projCoords.st + vec2( x, y ) * texelSize;
            uv.x = viewport.x + uv.x * viewport.z;
            uv.y = viewport.y + uv.y * viewport.w;
            float pcfDepth = texture( uShadowAtlas, uv ).x;
            shadow += depth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

                                float calculatePointShadow( sampler2D shadowAtlas, float dist, vec3 D, vec4 viewport, float bias )
                                {
                                    float depth = dist;
                                    float shadow = textureCubeUV( shadowAtlas, D, viewport ).r;
                                    return depth <= ( shadow + bias ) ? 0.0 : 1.0;
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

                                    vec4 positionData = texture( uPositions, uv );

                                    vec3 P = positionData.xyz;
                                    float viewSpacePositionDepth = positionData.w;
                                    vec3 N = texture( uNormals, uv ).xyz;
                                    vec4 material = texture( uMaterials, uv );
                                    float metallic = material.x;
                                    float roughness = material.y;
                                    float ambientOcclusion = material.z;

                                    vec3 E = inverse( view )[ 3 ].xyz;
                                    vec3 V = normalize( E - P );

                                    vec3 F0 = vec3( 0.04 );
                                    F0 = mix( F0, albedo, metallic );

                                    vec3 L = vec3( 0 );
                                    vec3 radiance = vec3( 0 );
                                    float shadow = 0.0;

                                    if ( uLight.type == 2 ) {
                                        // directional
                                        L = normalize( -uLight.direction.xyz );
                                        radiance = uLight.energy * uLight.color.rgb;

                                        if ( uLight.castShadows == 1 ) {
                                            vec4 cascadeSplits = uLight.cascadeSplits;

                                            // these are all negative values. Lower means farther away from the eye
                                            float depth = viewSpacePositionDepth;
                                            int cascadeId = 0;
                                            if ( depth < cascadeSplits[ 0 ] ) {
                                                cascadeId = 1;
                                            }
                                            if ( depth < cascadeSplits[ 1 ] ) {
                                                cascadeId = 2;
                                            }
                                            if ( depth < cascadeSplits[ 2 ] ) {
                                                cascadeId = 3;
                                            }

                                            mat4 lightSpaceMatrix = uLight.lightSpaceMatrix[ cascadeId ];
                                            vec4 viewport = uLight.viewport;
                                            vec2 viewportSize = viewport.zw;
                                            viewport.z = 0.5 * viewportSize.x;
                                            viewport.w = 0.5 * viewportSize.y;
                                            if ( cascadeId == 1 ) {
                                                // top-right cascade
                                                viewport.x += 0.5 * viewportSize.x;
                                            } else if ( cascadeId == 2 ) {
                                                // bottom-left cascade
                                                viewport.y += 0.5 * viewportSize.y;
                                            } else if ( cascadeId == 3 ) {
                                                // bottom-rigth cascade
                                                viewport.x += 0.5 * viewportSize.x;
                                                viewport.y += 0.5 * viewportSize.y;
                                            }

                                            vec4 lightSpacePos = ( bias * lightSpaceMatrix * vec4( P, 1.0 ) );
                                            if ( lightSpacePos.z >= -1.0 && lightSpacePos.z <= 1.0 ) {
                                                shadow = calculateShadow( lightSpacePos, viewport, N, L, 0.005 );
                                            } else {
                                                // the projected position in light space is outside the light's view frustum.
                                                // For directional lights, this means the object will be never in shadows
                                                // This might not be true for other types of lights. For example, a spot light
                                                // will produce shadows on objects outside its cone of influence
                                            }
                                        }
                                    } else if ( uLight.type == 3 ) {
                                        // spot
                                        L = uLight.position.xyz - P;
                                        float dist = length( L );
                                        L = normalize( L );

                                        float theta = dot( L, normalize( -uLight.direction.xyz ) );
                                        float epsilon = uLight.cutoff.x - uLight.cutoff.y;
                                        float intensity = clamp( ( theta - uLight.cutoff.y ) / epsilon, 0.0, 1.0 );
                                        float attenuation = 1.0 / ( dist * dist );
                                        radiance = attenuation * intensity * uLight.energy * uLight.color.rgb;

                                        if ( uLight.castShadows == 1 ) {
                                            vec4 lightSpacePos = ( bias * uLight.lightSpaceMatrix[ 0 ] * vec4( P, 1.0 ) );
                                            shadow = calculateShadow( lightSpacePos, uLight.viewport, N, L, 0.005 );
                                        }
                                    } else {
                                        L = uLight.position.xyz - P;
                                        float dist = length( L );
                                        L = normalize( L );
                                        float attenuation = 1.0 / ( dist * dist );
                                        radiance = uLight.color.rgb * uLight.energy * attenuation;

                                        if ( uLight.castShadows == 1 ) {
                                            shadow = calculatePointShadow(
                                                uShadowAtlas,
                                                dist,
                                                -L,
                                                uLight.viewport,
                                                uLight.shadowBias
                                            );
                                        }
                                    }

                                    vec3 H = normalize( V + L );
                                    vec3 Lo = brdf( N, H, V, L, radiance, albedo, metallic, roughness );

                                    Lo *= 1.0 - shadow;

                                    outColor = vec4( Lo, 1.0 );
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
        .dimensions = Rectf( 0, 0, 1, 1 ),
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
                    } else {
                        // TODO: Use instancing to render all lights in one call
                        commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pointLightPipeline ) );
                        commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
                        commandBuffer->bindDescriptorSet( light->getDescriptors() );
                        commandBuffer->drawPrimitive( crimild::get_ptr( lightVolume ) );
                    }
                } );
        } );
}
