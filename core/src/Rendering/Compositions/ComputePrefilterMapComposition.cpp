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

#include "Rendering/Compositions/ComputePrefilterMapComposition.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Frustum.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Visitors/ApplyToGeometries.hpp"

using namespace crimild;
using namespace crimild::compositions;

// Computes prefilter from a composition which outputs is a reflection map of the scene
// (possibly computed by computeReflectionMap())
// TODO (hernan): This composition should be executed only once at the begining of the simulation. There's no need
// to execute it multiple times if the reflection map does not change. But the frame graph does not allow that at
// the moment
Composition crimild::compositions::computePrefilterMap( Composition cmp ) noexcept
{
    auto useHDR = cmp.isHDREnabled();

    auto geometry = cmp.create< Geometry >();
    geometry->attachPrimitive(
        crimild::alloc< BoxPrimitive >(
            BoxPrimitive::Params {
                .type = Primitive::Type::TRIANGLES,
                .layout = VertexP3::getLayout(),
                .size = Vector3f( 10.0f, 10.0f, 10.0f ),
                .invertFaces = true,
            } ) );

    auto pipeline = [ & ] {
        auto pipeline = cmp.create< Pipeline >();
        pipeline->program = [ & ] {
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
                                        prefilteredColor += textureCubeUV( uHDRMap, L, viewport ).rgb * NdotL;
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
        }();
        pipeline->viewport = { .scalingMode = ScalingMode::DYNAMIC };
        pipeline->scissor = { .scalingMode = ScalingMode::DYNAMIC };
        return pipeline;
    }();

    auto environmentDescriptors = [ & ] {
        auto descriptors = cmp.create< DescriptorSet >();
        descriptors->descriptors = {
            {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = [ & ] {
                    auto texture = crimild::alloc< Texture >();
                    texture->imageView = cmp.getAttachment( "reflectionMapColor" )->imageView;
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

    auto recordProbeCommands = []( compositions::Composition &cmp, CommandBuffer *commandBuffer, Pipeline *pipeline, Array< ViewportDimensions > &layout, Size offset, Real32 roughness, Primitive *primitive, DescriptorSet *environmentDescriptors ) {
        auto viewports = ViewportDimensions::cubeViewportsFrom( layout[ offset++ ] );

        auto pMatrix = Frustumf( 90.0f, 1.0f, 0.1f, 200.0f ).computeProjectionMatrix();

        for ( auto face = 0l; face < 6; ++face ) {
            auto viewport = viewports[ face ];

            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            commandBuffer->bindGraphicsPipeline( pipeline );
            commandBuffer->bindDescriptorSet(
                [ & ] {
                    auto ds = cmp.create< DescriptorSet >();
                    ds->descriptors = {
                        {
                            .descriptorType = DescriptorType::UNIFORM_BUFFER,
                            .obj = [ & ] {
                                return crimild::alloc< CallbackUniformBuffer< Matrix4f > >(
                                    [ face, pMatrix ] {
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
                                        return vMatrix * pMatrix;
                                    } );
                            }(),
                        },
                    };
                    return ds;
                }() );
            commandBuffer->bindDescriptorSet( environmentDescriptors );
            commandBuffer->bindDescriptorSet(
                [ & ] {
                    auto ds = cmp.create< DescriptorSet >();
                    ds->descriptors = {
                        {
                            .descriptorType = DescriptorType::UNIFORM_BUFFER,
                            .obj = crimild::alloc< UniformBuffer >( Vector4f( roughness, 0, 0, 0 ) ),
                        },
                    };
                    return ds;
                }() );
            commandBuffer->drawPrimitive( crimild::get_ptr( primitive ) );
        }

        return offset;
    };

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

    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [ & ] {
            auto att = cmp.createAttachment( "prefilteredMapColor" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            if ( useHDR ) {
                att->format = Format::R32G32B32A32_SFLOAT;
            } else {
                att->format = Format::R8G8B8A8_UNORM;
            }
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }(),
    };

    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 768.0f,
        .height = 512.0f,
    };

    renderPass->commands = [ & ] {
        auto commandBuffer = crimild::alloc< CommandBuffer >();

        for ( auto offset = 0l; offset < 5; ++offset ) {
            recordProbeCommands(
                cmp,
                crimild::get_ptr( commandBuffer ),
                pipeline,
                viewportLayout,
                offset,
                Real32( offset ) / 4.0,
                geometry->anyPrimitive(),
                environmentDescriptors );
        }

        return commandBuffer;
    }();

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
