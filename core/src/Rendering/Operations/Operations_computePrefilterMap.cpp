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
#include "Mathematics/Frustum.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/UniformBuffer.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::computePrefilterMap( SharedPointer< FrameGraphResource > const reflectionAtlas ) noexcept
{
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "computePrefilterMap" );

    auto color = useColorAttachment( renderPass->getName() + "/color", Format::R32G32B32A32_SFLOAT );

    renderPass->attachments = { color };

    auto withRelativeDimensions = []( auto x, auto y, auto w, auto h ) {
        return ViewportDimensions {
            .scalingMode = ScalingMode::RELATIVE,
            .dimensions = Rectf( x, y, w, h ),
        };
    };

    // TODO: this layout is fixed since the number of mipmaps is known at compile time
    // I can pack each level in the empty spaces of the highest mip level
    auto viewportLayout = Array< ViewportDimensions > {
        // The sizes are relative. Texture resolution might change and we
        // don't need to recompute these values (unless aspect changes).
        withRelativeDimensions( 0, 0, 0.6666666667, 1.0 ),
        withRelativeDimensions( 0.6666666667, 0, 0.3333333333, 0.3333333333 ),
        withRelativeDimensions( 0.6666666667, 0.3333333333, 0.1666666667, 0.1666666667 ),
        withRelativeDimensions( 0.6666666667, 0.5, 0.08333333333, 0.08333333333 ),
        withRelativeDimensions( 0.6666666667, 0.5833333333, 0.04166666667, 0.04166666667 ),
    };

    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 768.0f,
        .height = 512.0f,
    };

    renderPass->reads( { reflectionAtlas } );
    renderPass->writes( { color } );
    renderPass->produces( { color } );

    auto primitive = crimild::alloc< BoxPrimitive >(
        BoxPrimitive::Params {
            .type = Primitive::Type::TRIANGLES,
            .layout = VertexP3::getLayout(),
            .size = Vector3f( 10.0f, 10.0f, 10.0f ),
            .invertFaces = true,
        } );

    auto pipeline = [ & ] {
        auto pipeline = crimild::alloc< GraphicsPipeline >();
        pipeline->setProgram(
            [ & ] {
                auto program = crimild::alloc< ShaderProgram >(
                    Array< SharedPointer< Shader > > {
                        crimild::alloc< Shader >(
                            Shader::Stage::VERTEX,
                            R"(
                            layout ( location = 0 ) in vec3 inPosition;

                            layout ( set = 0, binding = 0 ) uniform Uniforms {
                                mat4 probeSpaceMatrix;
                            };

                            layout ( location = 0 ) out vec3 outPosition;

                            void main()
                            {
                                gl_Position = probeSpaceMatrix * vec4( inPosition, 1.0 );
                                gl_Position = gl_Position.xyww;
                                outPosition = inPosition;
                            }
                        )" ),
                        crimild::alloc< Shader >(
                            Shader::Stage::FRAGMENT,
                            R"(
                            layout ( location = 0 ) in vec3 inPosition;

                            layout ( set = 1, binding = 0 ) uniform sampler2D uHDRMap;

                            layout ( set = 2, binding = 0 ) uniform Params {
                                vec4 params;
                            };

                            layout ( location = 0 ) out vec4 outColor;

                            const float PI = 3.14159265359;

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

vec4 textureCubeUV( sampler2D envMap, vec3 direction, vec4 viewport, int mipLevel )
{
    const float faceSize = 0.25;
    const vec2 texelSize = 1.0 / textureSize( envMap, mipLevel );

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

    vec4 color = textureLod( envMap, uv, mipLevel );
    return color;
}

                            // Compute Van Der Corpus sequence
                            float radicalInverseVdC( uint bits )
                            {
                                bits = ( bits << 16u ) | ( bits >> 16u );
                                bits = ( ( bits & 0x55555555u ) << 1u ) | ( ( bits & 0xAAAAAAAAu ) >> 1u );
                                bits = ( ( bits & 0x33333333u ) << 2u ) | ( ( bits & 0xCCCCCCCCu ) >> 2u );
                                bits = ( ( bits & 0x0F0F0F0Fu ) << 4u ) | ( ( bits & 0xF0F0F0F0u ) >> 4u );
                                bits = ( ( bits & 0x00FF00FFu ) << 8u ) | ( ( bits & 0xFF00FF00u ) >> 8u );
                                return float( bits ) * 2.3283064365386963e-10; // / 0x100000000
                            }

                            vec2 hammersley( uint i, uint N )
                            {
                                return vec2( float( i ) / float( N ), radicalInverseVdC( i ) );
                            }

                            vec3 importanceSampleGGX( vec2 xi, vec3 N, float roughness )
                            {
                                float a = roughness * roughness;
                                float phi = 2.0 * PI * xi.x;
                                float cosTheta = sqrt( ( 1.0 - xi.y ) / ( 1.0 + ( a * a - 1.0 ) * xi.y ) );
                                float sinTheta = sqrt( 1.0 - cosTheta * cosTheta );

                                // form spherical coordinates to cartesian coordinates
                                vec3 H;
                                H.x = cos( phi ) * sinTheta;
                                H.y = sin( phi ) * sinTheta;
                                H.z = cosTheta;

                                // from tangent-space vector to world-space sample vector
                                vec3 up = abs( N.z ) < 0.999 ? vec3( 0.0, 0.0, 1.0 ) : vec3( 1.0, 0.0, 0.0 );
                                vec3 tangent = normalize( cross( up, N ) );
                                vec3 bitangent = cross( N, tangent );

                                vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
                                return normalize( sampleVec );
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

                            void main() {
                             	vec4 viewport = vec4( 0, 0, 1, 1 );

                                vec3 N = normalize( inPosition );
                                vec3 R = N;
                                vec3 V = R;

                                float roughness = params.x;

                                const uint sampleCount = 1024u;
                                float totalWeight = 0.0;
                                vec3 prefilteredColor = vec3( 0.0 );
                                for ( uint i = 0; i < sampleCount; ++i ) {
                                    vec2 xi = hammersley( i, sampleCount );
                                    vec3 H = importanceSampleGGX( xi, N, roughness );
                                    vec3 L = normalize( 2.0 * dot( V, H ) * H - V );

                                    float NdotL = max( dot( N, L ), 0.0 );
                                    if ( NdotL > 0.0 ) {
            							// Sample from environment's mip level based on roughness/pdf
                   						float D = distributionGGX( N, H, roughness );
                        				float NdotH = max( dot( N, H ), 0.0 );
                         				float HdotV = max( dot( H, V ), 0.0 );
                         				float pdf = D * NdotH / ( 4.0 * HdotV ) + 0.0001;

                         				float resolution = textureSize( uHDRMap, 0 ).r / 4.0; // Face resolution
                         				float saTexel = 4.0 * PI / ( 6.0 * resolution * resolution );
                         				float saSample = 1.0 / ( float( sampleCount ) * pdf + 0.0001 );

                         				float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2( saSample / saTexel );

                                        prefilteredColor += textureCubeUV( uHDRMap, L, viewport, int( mipLevel ) ).rgb * NdotL;
                                        totalWeight += NdotL;
                                    }
                                }

                                prefilteredColor = prefilteredColor / totalWeight;
                                outColor = vec4( prefilteredColor, 1.0 );
                            }
                        )" ) } );
                program->vertexLayouts = { VertexLayout::P3 };
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
                                .stage = Shader::Stage::FRAGMENT,
                            },
                        };
                        return layout;
                    }(),
                };
                return program;
            }() );
        pipeline->viewport = { .scalingMode = ScalingMode::DYNAMIC };
        pipeline->scissor = { .scalingMode = ScalingMode::DYNAMIC };
        return pipeline;
    }();

    auto environmentDescriptors = [ & ] {
        auto descriptors = crimild::alloc< DescriptorSet >();
        descriptors->descriptors = {
            {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = [ & ] {
                    auto texture = withResource( crimild::alloc< Texture >(), reflectionAtlas );
                    texture->sampler = [ & ] {
                        auto sampler = crimild::alloc< Sampler >();
                        sampler->setMinFilter( Sampler::Filter::LINEAR );
                        sampler->setMagFilter( Sampler::Filter::LINEAR );
                        sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
                        sampler->setCompareOp( CompareOp::NEVER );
                        return sampler;
                    }();
                    return texture;
                }(),
            },
        };
        return descriptors;
    }();

    auto pMatrix = Frustumf( 90.0f, 1.0f, 0.1f, 200.0f ).computeProjectionMatrix();
    auto descriptors = Array< SharedPointer< DescriptorSet > >( 6 );
    for ( auto face = 0l; face < 6; ++face ) {
        auto ds = crimild::alloc< DescriptorSet >();
        ds->descriptors = {
            {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = [ & ] {
                    struct Props {
                        Matrix4f view;
                        Matrix4f proj;
                    };

                    Transformation t;
                    switch ( face ) {
                        case 0: // positive x
                            t.rotate().fromAxisAngle( Vector3f::UNIT_Y, Numericf::HALF_PI );
                            break;

                        case 1: // negative x
                            t.rotate().fromAxisAngle( Vector3f::UNIT_Y, -Numericf::HALF_PI );
                            break;

                        case 2: // positive y
                            t.rotate().fromAxisAngle( Vector3f::UNIT_X, Numericf::HALF_PI );
                            break;

                        case 3: // negative y
                            t.rotate().fromAxisAngle( Vector3f::UNIT_X, -Numericf::HALF_PI );
                            break;

                        case 4: // positive z
                            t.rotate().fromAxisAngle( Vector3f::UNIT_Y, Numericf::PI );
                            break;

                        case 5: // negative z
                            t.rotate().fromAxisAngle( Vector3f::UNIT_Y, 0 );
                            break;
                    }

                    t.setTranslate( Vector3f::ZERO ); // TODO (hernan): use probe's position
                    auto vMatrix = t.computeModelMatrix().getInverse();

                    return crimild::alloc< UniformBuffer >(
                        Props {
                            .view = vMatrix,
                            .proj = pMatrix,
                        } );
                }(),
            },
        };
        descriptors[ face ] = ds;
    }

    auto levelDescriptors = Array< SharedPointer< DescriptorSet > >( 5 );
    for ( auto level = 0l; level < levelDescriptors.size(); ++level ) {
        auto roughness = Real32( level ) / Real32( levelDescriptors.size() - 1 );
        auto ds = crimild::alloc< DescriptorSet >();
        ds->descriptors = {
            {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = crimild::alloc< UniformBuffer >( Vector4f( roughness, 0, 0, 0 ) ),
            },
        };
        levelDescriptors[ level ] = ds;
    }

    return withConditionalGraphicsCommands(
        renderPass,
        [] {
            // only render once when forced by render system reset
            return false;
        },
        [ descriptors,
          levelDescriptors,
          environmentDescriptors,
          primitive,
          pipeline,
          viewportLayout ]( auto commandBuffer ) {
            for ( auto level = 0l; level < levelDescriptors.size(); ++level ) {
                auto viewports = ViewportDimensions::cubeViewportsFrom( viewportLayout[ level ] );
                for ( auto face = 0l; face < 6; ++face ) {
                    auto viewport = viewports[ face ];

                    commandBuffer->setViewport( viewport );
                    commandBuffer->setScissor( viewport );
                    commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
                    commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors[ face ] ) );
                    commandBuffer->bindDescriptorSet( crimild::get_ptr( environmentDescriptors ) );
                    commandBuffer->bindDescriptorSet( crimild::get_ptr( levelDescriptors[ level ] ) );
                    commandBuffer->drawPrimitive( crimild::get_ptr( primitive ) );
                }
            }
        } );
}
