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

#include "Rendering/Operations/Operations_ssao.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Interpolation.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::ssao( SharedPointer< FrameGraphResource > positions, SharedPointer< FrameGraphResource > normals ) noexcept
{
    // TODO: move this to a compute pass
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "ssao" );

    auto color = useColorAttachment( "ssao/color" );

    renderPass->attachments = { color };

    auto pipeline = [ & ] {
        auto pipeline = crimild::alloc< GraphicsPipeline >();
        pipeline->setProgram(
            [] {
                auto program = crimild::alloc< ShaderProgram >(
                    Array< SharedPointer< Shader > > {
                        crimild::alloc< Shader >(
                            Shader::Stage::VERTEX,
                            CRIMILD_TO_STRING(
                                vec2 positions[ 6 ] = vec2[](
                                    vec2( -1.0, 1.0 ),
                                    vec2( -1.0, -1.0 ),
                                    vec2( 1.0, -1.0 ),

                                    vec2( -1.0, 1.0 ),
                                    vec2( 1.0, -1.0 ),
                                    vec2( 1.0, 1.0 ) );

                                vec2 texCoords[ 6 ] = vec2[](
                                    vec2( 0.0, 0.0 ),
                                    vec2( 0.0, 1.0 ),
                                    vec2( 1.0, 1.0 ),

                                    vec2( 0.0, 0.0 ),
                                    vec2( 1.0, 1.0 ),
                                    vec2( 1.0, 0.0 ) );

                                layout( location = 0 ) out vec2 outTexCoord;

                                void main() {
                                    gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
                                    outTexCoord = texCoords[ gl_VertexIndex ];
                                } ) ),
                            crimild::alloc< Shader >(
                                Shader::Stage::FRAGMENT,
                                R"(
                                    layout( location = 0 ) in vec2 inTexCoord;

                                layout( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                    mat4 view;
                                    mat4 proj;
                                };

                                    layout( set = 0, binding = 1 ) uniform sampler2D uPositionMap;
                                    layout( set = 0, binding = 2 ) uniform sampler2D uNormalMap;

                                    layout( set = 0, binding = 3 ) uniform Uniforms {
                                        float radius;
                                        float bias;
                                        vec3 samples[ 64 ];
                                    };

                                    layout( set = 0, binding = 4 ) uniform sampler2D uNoiseMap;

                                    layout( location = 0 ) out vec4 outColor;

                                    void main() {

vec2 screenSize = textureSize( uPositionMap, 0 );

vec4 positionData = texture( uPositionMap, inTexCoord );
vec4 normalData = texture( uNormalMap, inTexCoord );

vec3 P = vec3( view * vec4( positionData.xyz, 1.0 ) );
float D = positionData.w;
vec3 N = vec3( view * vec4( normalData.xyz, 0.0 ) );

vec2 noiseSize = textureSize( uNoiseMap, 0 );
vec2 noiseScale = screenSize / noiseSize;
vec3 randomVec = texture( uNoiseMap, noiseScale * inTexCoord ).xyz;

vec3 tangent = normalize( randomVec - N * dot( randomVec, N ) );
vec3 bitangent = cross( N, tangent );
mat3 TBN = mat3( tangent, bitangent, N );

float occlusion = 0.0;
int sampleCount = 64;

for ( int i = 0; i < sampleCount; ++i ) {
    vec3 samplePos = TBN * samples[ i ];
    samplePos = P + samplePos * radius;

    vec4 offset = vec4( samplePos, 1.0 );
    offset = proj * offset;
    offset.xyz /= offset.w;
    offset.xyz = offset.xyz * 0.5 + 0.5;
    offset.y = 1.0 - offset.y;

    float sampleDepth = ( view * vec4( texture( uPositionMap, offset.xy ).xyz, 1.0 ) ).z;

    float rangeCheck = smoothstep( 0.0, 1.0, radius / abs( P.z - sampleDepth ) );

    occlusion += ( sampleDepth >= samplePos.z + bias ? 1.0 : 0.0 ) * rangeCheck;
}


occlusion = 1.0 - occlusion / sampleCount;

vec3 color = vec3( occlusion );


                                        outColor = vec4( color, 1.0 );
                                    } )" ),
                    } );
                program->descriptorSetLayouts = {
                    [] {
                        auto layout = crimild::alloc< DescriptorSetLayout >();
                        layout->bindings = {
                            {
                                // samples
                                .descriptorType = DescriptorType::UNIFORM_BUFFER,
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
                                // samples
                                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                .stage = Shader::Stage::FRAGMENT,
                            },
                            {
                                // noise texture
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

    auto descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                // TODO: get camera that was used for rendering
                .obj = crimild::alloc< CameraViewProjectionUniform >( Camera::getMainCamera() ),
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
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = [ & ] {
                    auto settings = Simulation::getInstance()->getSettings();

                    struct Uniforms {
                        alignas( 4 ) Real32 radius;
                        alignas( 4 ) Real32 bias;
                        alignas( 4 ) Real32 renderScale;
                        alignas( 16 ) Vector3f samples[ 64 ];
                    };

                    auto data = Uniforms {
                        .radius = settings->get< Real32 >( "video.ssao.radius", 5.0 ),
                        .bias = settings->get< Real32 >( "video.ssao.bias", 0.05 ),
                        .renderScale = settings->get< Real32 >( "video.ssao.renderScale", 0.5f ),
                    };

                    std::uniform_real_distribution< Real32 > distribution( 0.0f, 1.0f );
                    std::default_random_engine generator;
                    for ( auto i = 0l; i < 64; ++i ) {
                        auto sample = Vector3f(
                            distribution( generator ) * 2.0f - 1.0f,
                            distribution( generator ) * 2.0f - 1.0f,
                            distribution( generator ) );
                        sample.normalize();
                        //sample *= distribution( generator );
                        // Place larger weights on samples closer to the fragment
                        auto scale = Real32( i ) / 64.0f;
                        Interpolation::linear( 0.1f, 1.0f, scale * scale, scale );
                        sample *= scale;
                        data.samples[ i ] = sample;
                    }
                    return crimild::alloc< UniformBuffer >( data );
                }(),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = [] {
                    auto texture = crimild::alloc< Texture >();
                    texture->imageView = [] {
                        // Kernel rotation texture
                        auto imageView = crimild::alloc< ImageView >();
                        imageView->image = [] {
                            //auto data = ByteArray( 16 * 4 );
                            auto data = Array< Vector4f >( 16 );
                            std::uniform_real_distribution< Real32 > distribution( 0.0f, 1.0f );
                            std::default_random_engine generator;
                            for ( auto i = 0l; i < data.size(); ++i ) {
                                data[ i ] = Vector4f(
                                    distribution( generator ) * 2.0f - 1.0f,
                                    distribution( generator ) * 2.0f - 1.0f,
                                    0,
                                    0 );
                            }

                            auto image = crimild::alloc< Image >();
                            image->extent = {
                                .width = 4,
                                .height = 4,
                                .depth = 1,
                            };
                            image->format = Format::R32G32B32A32_SFLOAT;
                            image->data = [ & ] {
                                auto bytes = ByteArray( data.size() * sizeof( Vector4f ) );
                                memcpy( bytes.getData(), data.getData(), bytes.size() );
                                return bytes;
                            }();
                            return image;
                        }();
                        return imageView;
                    }();
                    texture->sampler = [] {
                        auto sampler = crimild::alloc< Sampler >();
                        sampler->setMinFilter( Sampler::Filter::NEAREST );
                        sampler->setMagFilter( Sampler::Filter::NEAREST );
                        sampler->setWrapMode( Sampler::WrapMode::REPEAT );
                        return sampler;
                    }();
                    return texture;
                }(),
            },
        };
        return descriptorSet;
    }();

    auto settings = Simulation::getInstance()->getSettings();

    renderPass->extent = {
        .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
        .width = settings->get< Real32 >( "video.ssao.renderScale", 0.5f ),
        .height = settings->get< Real32 >( "video.ssao.renderScale", 0.5f ),
    };

    renderPass->reads( { positions, normals } );
    renderPass->writes( { color } );
    renderPass->produces( { color } );

    auto viewport = ViewportDimensions {
        .scalingMode = ScalingMode::RELATIVE,
    };

    return withGraphicsCommands(
        renderPass,
        [ pipeline, descriptors, viewport ]( auto commandBuffer ) {
            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
            commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
            commandBuffer->draw( 6 );
        } );
}
