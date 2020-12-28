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

#include "Rendering/Compositions/GaussianBlurComposition.hpp"

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;
using namespace crimild::compositions;

Composition crimild::compositions::gaussianBlur( Composition cmp ) noexcept
{
    auto createPipeline = [ & ]( bool horizontal ) {
        auto pipeline = cmp.create< GraphicsPipeline >();
        pipeline->setProgram(
            [ & ] {
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
                                horizontal
                                    ? CRIMILD_TO_STRING(
                                        layout( location = 0 ) in vec2 inTexCoord;

                                        layout( set = 0, binding = 0 ) uniform sampler2D uColorMap;

                                        layout( location = 0 ) out vec4 outColor;

                                        void main() {
                                            const float weights[ 5 ] = float[]( 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 );
                                            vec2 offset = 1.0 / textureSize( uColorMap, 0 );
                                            vec3 result = texture( uColorMap, inTexCoord ).rgb;
                                            for ( int i = 0; i < 5; ++i ) {
                                                result += weights[ i ] * texture( uColorMap, inTexCoord + vec2( offset.x * i, 0.0 ) ).rgb;
                                                result += weights[ i ] * texture( uColorMap, inTexCoord - vec2( offset.x * i, 0.0 ) ).rgb;
                                            }
                                            outColor = vec4( result, 1.0 );
                                        } )
                                    : CRIMILD_TO_STRING(
                                        layout( location = 0 ) in vec2 inTexCoord;

                                        layout( set = 0, binding = 1 ) uniform sampler2D uColorMap;

                                        layout( location = 0 ) out vec4 outColor;

                                        void main() {
                                            const float weights[ 5 ] = float[]( 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 );
                                            vec2 offset = 1.0 / textureSize( uColorMap, 0 );
                                            vec3 result = texture( uColorMap, inTexCoord ).rgb;
                                            for ( int i = 0; i < 5; ++i ) {
                                                result += weights[ i ] * texture( uColorMap, inTexCoord + vec2( 0.0, offset.y * i ) ).rgb;
                                                result += weights[ i ] * texture( uColorMap, inTexCoord - vec2( 0.0, offset.y * i ) ).rgb;
                                            }
                                            outColor = vec4( result, 1.0 );
                                        } ) ),
                    } );
                program->descriptorSetLayouts = {
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
    };

    auto horizontalPipeline = createPipeline( true );
    auto verticalPipeline = createPipeline( false );

    auto pass = []( Composition cmp, auto pipeline ) {
        auto renderPass = cmp.create< RenderPass >();
        renderPass->attachments = {
            [ & ] {
                auto att = cmp.createAttachment( "gaussianBlur" );
                att->usage = Attachment::Usage::COLOR_ATTACHMENT;
                if ( cmp.isHDREnabled() ) {
                    att->format = Format::R32G32B32A32_SFLOAT;
                } else {
                    att->format = Format::R8G8B8A8_UNORM;
                }
                att->imageView = crimild::alloc< ImageView >();
                att->imageView->image = crimild::alloc< Image >();
                return crimild::retain( att );
            }(),
        };

        auto descriptors = [ & ] {
            auto descriptorSet = cmp.create< DescriptorSet >();
            descriptorSet->descriptors = {
                {
                    .descriptorType = DescriptorType::TEXTURE,
                    .obj = [ & ] {
                        if ( cmp.getOutputTexture() != nullptr ) {
                            return crimild::retain( cmp.getOutputTexture() );
                        }
                        auto texture = crimild::alloc< Texture >();
                        texture->imageView = cmp.getOutput()->imageView;
                        texture->sampler = [] {
                            auto sampler = crimild::alloc< Sampler >();
                            // Use linear filtering to help with the bluring effect
                            sampler->setMinFilter( Sampler::Filter::LINEAR );
                            sampler->setMagFilter( Sampler::Filter::LINEAR );
                            sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_EDGE );
                            return sampler;
                        }();
                        return texture;
                    }(),
                },
            };
            return descriptorSet;
        }();

        // Use half-screen resolution
        renderPass->extent = {
            .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
            .width = 0.5f,
            .height = 0.5f,
        };

        auto viewport = ViewportDimensions {
            .scalingMode = ScalingMode::RELATIVE,
        };

        renderPass->commands = [ & ] {
            auto commandBuffer = crimild::alloc< CommandBuffer >();
            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            commandBuffer->bindGraphicsPipeline( pipeline );
            commandBuffer->bindDescriptorSet( descriptors );
            commandBuffer->draw( 6 );
            return commandBuffer;
        }();

        cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

        return cmp;
    };

    Composition ret = cmp;
    ret = pass( ret, horizontalPipeline );
    ret = pass( ret, verticalPipeline );

    return ret;
}
