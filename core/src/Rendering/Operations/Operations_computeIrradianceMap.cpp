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

SharedPointer< FrameGraphOperation > crimild::framegraph::computeIrradianceMap( SharedPointer< FrameGraphResource > const reflectionAtlas ) noexcept
{
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "computeIrradianceMap" );

    auto color = useColorAttachment( renderPass->getName() + "/color", Format::R32G32B32A32_SFLOAT );

    renderPass->attachments = { color };

    // TODO: when creating the probe atlas, we can compute
    // a different layout here
    auto viewportLayout = Array< ViewportDimensions > {
        {
            .scalingMode = ScalingMode::RELATIVE,
            .dimensions = Rectf( 0, 0, 1, 1 ),
        },
    };

    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 256.0f,
        .height = 256.0f,
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

                            // Performs bilinear filtering
vec4 textureCubeUV( sampler2D envMap, vec3 direction, vec4 viewport )
{
    const float faceSize = 0.25;
    const vec2 texelSize = 1.0 / textureSize( envMap, 0 );

    float face = getFace( direction );
    vec2 faceOffsets = getFaceOffsets( face );


    vec2 uv = getUV( direction, face );
    vec2 f = fract( uv );
    uv.y = 1.0 - uv.y;
    uv = faceOffsets + faceSize * uv;
    uv = viewport.xy + uv * viewport.zw;
    vec2 fBL = faceOffsets * viewport.zw + texelSize;
    vec2 fTR = fBL + faceSize * viewport.zw - 2.0 * texelSize;

    uv = max( uv, fBL );
    uv = min( uv, fTR );

    vec4 color = texture( envMap, uv );
    return color;
}

                            void main() {
                             	vec4 viewport = vec4( 0, 0, 1, 1 );

                            	vec3 N = normalize( inPosition );
                             	vec3 U = vec3( 0, 1, 0 );
                              	vec3 R = cross( U, N );
                               	U = cross( N, R );

                                vec3 irradiance = vec3( 0 );

								// TODO (hernan): set sampleDelta to 0.025 for better quality once I figure out
                                // how to do this step only once at the beginning of the simulation
								float sampleDelta = 0.05;
        						float nrSamples = 0.0;
              					for ( float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta ) {
                   					for ( float theta = 0.0f; theta < 0.5 * PI; theta += sampleDelta ) {
                        				// spherical to cartesian (in tangent space)
                                        vec3 tangentSample = vec3(
                                            sin( theta ) * cos( phi ),
                                            sin( theta ) * sin( phi ),
                                            cos( theta )
                                        );

                                        // tangent to world
                                        vec3 sampleVec = tangentSample.x * R + tangentSample.y * U + tangentSample.z * N;

                                        irradiance += textureCubeUV( uHDRMap, sampleVec, viewport ).rgb * cos( theta ) * sin( theta );
                                        nrSamples++;
                        			}
                                }

                            	irradiance = PI * irradiance / nrSamples;
                                outColor = vec4( irradiance, 1.0 );
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

    return withDynamicGraphicsCommands(
        renderPass,
        [ descriptors,
          environmentDescriptors,
          primitive,
          pipeline,
          viewportLayout ]( auto commandBuffer ) {
            auto viewports = ViewportDimensions::cubeViewportsFrom( viewportLayout[ 0 ] );

            for ( auto face = 0l; face < 6; ++face ) {
                auto viewport = viewports[ face ];

                commandBuffer->setViewport( viewport );
                commandBuffer->setScissor( viewport );
                commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
                commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors[ face ] ) );
                commandBuffer->bindDescriptorSet( crimild::get_ptr( environmentDescriptors ) );
                commandBuffer->drawPrimitive( crimild::get_ptr( primitive ) );
            }
        } );
}
