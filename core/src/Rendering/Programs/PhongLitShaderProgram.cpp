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

#include "Rendering/Programs/PhongLitShaderProgram.hpp"

#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Vertex.hpp"

using namespace crimild;

PhongLitShaderProgram::PhongLitShaderProgram( void ) noexcept
{
    setShaders(
        {
            crimild::alloc< Shader >(
                Shader::Stage::VERTEX,
                CRIMILD_TO_STRING(
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
                        outPosition = vec3( model * vec4( inPosition, 1.0 ) );
                        outViewPosition = ( view * model * vec4( inPosition, 1.0 ) ).xyz;
                        outEyePosition = inverse( view )[ 3 ].xyz;
                        outTexCoord = inTexCoord;
                    } ) ),
                crimild::alloc< Shader >(
                    Shader::Stage::FRAGMENT,
                    CRIMILD_TO_STRING(
                        layout( location = 0 ) in vec3 inNormal;
                        layout( location = 1 ) in vec3 inPosition;
                        layout( location = 2 ) in vec3 inViewPosition;
                        layout( location = 3 ) in vec2 inTexCoord;
                        layout( location = 4 ) in vec3 inEyePosition;
                        layout( location = 5 ) in mat3 inViewMatrix3;

                        struct LightProps {
                            uint type;
                            vec4 position;
                            vec4 direction;
                            vec4 ambient;
                            vec4 color;
                            vec4 attenuation;
                            vec4 cutoff;
                            bool castShadows;
                            vec4 cascadeSplits;
                            mat4 lightSpaceMatrix[ 4 ];
                            vec4 viewport;
                        };

                        layout( set = 0, binding = 1 ) uniform Lighting {
                            LightProps ambientLights[ 1 ];
                            uint ambientLightCount;
                            LightProps directionalLights[ 2 ];
                            uint directionalLightCount;
                            LightProps pointLights[ 10 ];
                            uint pointLightCount;
                            LightProps spotLights[ 4 ];
                            uint spotLightCount;
                        } lighting;

                        layout( set = 0, binding = 2 ) uniform sampler2D uShadowAtlas;

                        layout( set = 1, binding = 0 ) uniform Material {
                            vec4 ambient;
                            vec4 diffuse;
                            vec4 specular;
                            float shininess;
                        } material;

                        layout( set = 1, binding = 1 ) uniform sampler2D uDiffuseMap;
                        layout( set = 1, binding = 2 ) uniform sampler2D uSpecularMap;
                        layout( set = 1, binding = 3 ) uniform sampler2D uNormalMap;

                        layout( location = 0 ) out vec4 outColor;

                        vec3 ambient( vec3 lAmbient ) {
                            return lAmbient * material.ambient.rgb;
                        }

                        vec3 diffuse( vec3 N, vec3 L ) {
                            float d = max( dot( N, L ), 0.0 );
                            return vec3( d ) * material.diffuse.rgb;
                        }

                        vec3 specular( vec3 N, vec3 L, vec3 E, vec3 P, vec3 S ) {
                            vec3 V = normalize( E - P );
                            vec3 H = normalize( L + V );
                            float s = pow( max( dot( N, H ), 0.0 ), material.shininess );
                            return vec3( s ) * S;
                        }

                        float attenuation( float d, vec3 K ) {
                            return 1.0 / ( K.x + d * K.y + d * d * K.z );
                        }

                        const mat4 bias = mat4(
                            0.5, 0.0, 0.0, 0.0,
                            0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 1.0, 0.0,
                            0.5, 0.5, 0.0, 1.0 );

                        float linearizeDepth( float depth, float nearPlane, float farPlane ) {
                            float z = depth * 2.0 - 1.0; // Back to NDC
                            return ( 2.0 * nearPlane * farPlane ) / ( farPlane + nearPlane - z * ( farPlane - nearPlane ) ) / farPlane;
                        }

                        float calculateShadow( vec4 ligthSpacePosition, vec4 viewport, vec3 N, vec3 L, float bias ) {
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

                        const float FACE_INVALID = -1.0;
                        const float FACE_LEFT = 0.0;
                        const float FACE_RIGHT = 1.0;
                        const float FACE_FRONT = 2.0;
                        const float FACE_BACK = 3.0;
                        const float FACE_UP = 4.0;
                        const float FACE_DOWN = 5.0;

                        // Return the face in cubemap based on the principle component of the direction
                        float getFace( vec3 direction ) {
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

                        vec2 getUV( vec3 direction, float face ) {
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

                        vec2 getFaceOffsets( float face ) {
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

                        vec4 textureCubeUV( sampler2D envMap, vec3 direction, vec4 viewport ) {
                            float face = getFace( direction );
                            vec2 uv = getUV( direction, face );
                            uv.y = 1.0 - uv.y;
                            uv = getFaceOffsets( face ) + 0.25 * uv;
                            uv.x = viewport.x + uv.x * viewport.z;
                            uv.y = viewport.y + uv.y * viewport.w;
                            vec4 color = texture( envMap, uv );
                            return color;
                        }

                        float calculatePointShadow( sampler2D shadowAtlas, float dist, vec3 D, vec4 viewport ) {
                            float depth = dist / 200.0; //length( D ) / 200.0;
                            float shadow = textureCubeUV( shadowAtlas, D, viewport ).r;
                            return depth - 0.05 > shadow ? 1.0 : 0.0;

                            /*
                        // Compute shadow PCF
                        float bias = 0.05;
                        float shadow = 0.0f;

                        for ( int y = -1; y <= 1; ++y ) {
                            for ( int x = -1; x <= 1; ++x ) {
                                for ( int z = -1; z <= 1; ++z ) {
                                    float pcfDepth = textureCubeUV( uShadowAtlas, normalize( D + 0.01 * vec3( x, y, z ) ) ).r;// + vec3( x, y, z ) ).r;
                                    shadow += depth - bias > pcfDepth ? 1.0 : 0.0;
                                }
                            }
                        }
                        shadow /= 9.0;
                        return shadow;
                        */
                        }

                        // Computes T, B and TBN matrix based on surface N
                        // TODO (hernan): is this in view space? If so, we transform T and B to world space
                        mat3 computeTBN( mat3 invView, vec3 N ) {
                            vec3 q1 = dFdx( inPosition );
                            vec3 q2 = dFdy( inPosition );
                            vec2 st1 = dFdx( inTexCoord );
                            vec2 st2 = dFdy( inTexCoord );
                            vec3 T = normalize( invView * ( q1 * st2.t - q2 * st1.t ) );
                            vec3 B = -normalize( invView * ( -q1 * st2.s + q2 * st1.s ) );
                            return mat3( T, B, N );
                        }

                        void main() {
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
                            vec3 E = inEyePosition;
                            vec3 MS = material.specular.rgb * texture( uSpecularMap, inTexCoord ).rgb;

                            vec3 cascadeColor = vec3( 0 );

                            vec3 lightContribution = vec3( 0 );

                            // ambient lights
                            for ( uint i = 0; i < lighting.ambientLightCount; ++i ) {
                                vec3 A = ambient( lighting.ambientLights[ i ].ambient.rgb );
                                lightContribution += A;
                            }

                            // point lights
                            for ( uint i = 0; i < lighting.pointLightCount; ++i ) {
                                vec3 L = lighting.pointLights[ i ].position.xyz - inPosition;
                                float dist = length( L );
                                L = normalize( L );
                                vec3 Lc = lighting.pointLights[ i ].color.rgb;
                                vec3 A = ambient( lighting.pointLights[ i ].ambient.rgb );
                                vec3 D = diffuse( N, L );
                                vec3 S = specular( N, L, E, P, MS );
                                float lAtt = attenuation( dist, lighting.pointLights[ i ].attenuation.xyz );

                                if ( lighting.pointLights[ i ].castShadows ) {
                                    float shadow = calculatePointShadow( uShadowAtlas, dist, -L, lighting.pointLights[ i ].viewport );
                                    D *= 1.0 - shadow;
                                    S *= 1.0 - shadow;
                                }

                                lightContribution += Lc * lAtt * ( A + D + S );
                            }

                            // directional lights
                            for ( uint i = 0; i < lighting.directionalLightCount; ++i ) {
                                vec3 L = normalize( -lighting.directionalLights[ i ].direction.xyz );
                                vec3 Lc = lighting.directionalLights[ i ].color.rgb;
                                vec3 A = ambient( lighting.directionalLights[ i ].ambient.rgb );
                                vec3 D = diffuse( N, L );
                                vec3 S = specular( N, L, E, P, MS );

                                if ( lighting.directionalLights[ i ].castShadows ) {
                                    vec4 cascadeSplits = lighting.directionalLights[ i ].cascadeSplits;

                                    // these are all negative values. Lower means farther away from the eye
                                    float depth = inViewPosition.z;
                                    int cascadeId = 0;
                                    cascadeColor = vec3( 0.0, 1.0, 1.0 );
                                    if ( depth < cascadeSplits[ 0 ] ) {
                                        cascadeId = 1;
                                        cascadeColor = vec3( 1.0, 0.0, 0.0 );
                                    }
                                    if ( depth < cascadeSplits[ 1 ] ) {
                                        cascadeId = 2;
                                        cascadeColor = vec3( 1.0, 1.0, 0.0 );
                                    }
                                    if ( depth < cascadeSplits[ 2 ] ) {
                                        cascadeId = 3;
                                        cascadeColor = vec3( 0.0, 1.0, 0.0 );
                                    }

                                    mat4 lightSpaceMatrix = lighting.directionalLights[ i ].lightSpaceMatrix[ cascadeId ];
                                    vec4 viewport = lighting.directionalLights[ i ].viewport;
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

                                    vec4 lightSpacePos = ( bias * lightSpaceMatrix * vec4( inPosition, 1.0 ) );
                                    if ( lightSpacePos.z >= -1.0 && lightSpacePos.z <= 1.0 ) {
                                        float shadow = calculateShadow( lightSpacePos, viewport, N, L, 0.005 );
                                        D *= 1.0 - shadow;
                                        S *= 1.0 - shadow;
                                    } else {
                                        // the projected position in light space is outside the light's view frustum.
                                        // For directional lights, this means the object will be never in shadows
                                        // This might not be true for other types of lights. For example, a spot light
                                        // will produce shadows on objects outside its cone of influence
                                    }
                                }

                                lightContribution += Lc * ( A + D + S );
                            }

                            // spot light
                            for ( uint i = 0; i < lighting.spotLightCount; ++i ) {
                                vec3 L = lighting.spotLights[ i ].position.xyz - inPosition;
                                float dist = length( L );
                                L = normalize( L );
                                float theta = dot( L, normalize( -lighting.spotLights[ i ].direction.xyz ) );
                                float epsilon = lighting.spotLights[ i ].cutoff.x - lighting.spotLights[ i ].cutoff.y;
                                float intensity = clamp( ( theta - lighting.spotLights[ i ].cutoff.y ) / epsilon, 0.0, 1.0 );
                                vec3 Lc = lighting.spotLights[ i ].color.rgb;
                                vec3 A = ambient( lighting.spotLights[ i ].ambient.rgb );
                                vec3 D = intensity * diffuse( N, L );
                                vec3 S = intensity * specular( N, L, E, P, MS );
                                float lAtt = attenuation( dist, lighting.spotLights[ i ].attenuation.xyz );

                                if ( lighting.spotLights[ i ].castShadows ) {
                                    vec4 lightSpacePos = ( bias * lighting.spotLights[ i ].lightSpaceMatrix[ 0 ] * vec4( inPosition, 1.0 ) );
                                    float shadow = calculateShadow( lightSpacePos, lighting.spotLights[ i ].viewport, N, L, 0.005 );
                                    D *= 1.0 - shadow;
                                    S *= 1.0 - shadow;
                                }
                                lightContribution += Lc * lAtt * ( A + D + S );
                            }

                            vec4 color = texture( uDiffuseMap, inTexCoord );
                            if ( color.a <= 0.01 ) {
                                discard;
                            }

                            //color.rgb *= cascadeColor;

                            outColor = vec4( color.rgb * lightContribution, color.a );
                        } ) ),
        } );

    vertexLayouts = { VertexP3N3TC2::getLayout() };

    descriptorSetLayouts = {
        [] {
            auto layout = crimild::alloc< DescriptorSetLayout >();
            layout->bindings = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stage = Shader::Stage::VERTEX,
                },
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stage = Shader::Stage::ALL,
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
                    // Material properties
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Diffuse Map
                    .descriptorType = DescriptorType::TEXTURE,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Specular Map
                    .descriptorType = DescriptorType::TEXTURE,
                    .stage = Shader::Stage::FRAGMENT,
                },
                {
                    // Normal Map
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
                    .stage = Shader::Stage::VERTEX,
                },
            };
            return layout;
        }(),
    };
}
