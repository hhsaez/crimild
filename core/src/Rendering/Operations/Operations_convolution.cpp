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
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::blur( SharedPointer< FrameGraphResource > const &image ) noexcept
{
    return convolution(
        "blur",
        image,
        Matrix3f {
            1.0f / 16.0f,
            2.0f / 16.0f,
            1.0f / 16.0f,
            2.0f / 16.0f,
            4.0f / 16.0f,
            2.0f / 16.0f,
            1.0f / 16.0f,
            2.0f / 16.0f,
            1.0f / 16.0f,
        } );
}

SharedPointer< FrameGraphOperation > crimild::framegraph::sharpen( SharedPointer< FrameGraphResource > const &image ) noexcept
{
    return convolution(
        "sharpen",
        image,
        Matrix3f {
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            +9.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
        } );
}

SharedPointer< FrameGraphOperation > crimild::framegraph::edges( SharedPointer< FrameGraphResource > const &image ) noexcept
{
    return convolution(
        "edges",
        image,
        Matrix3f {
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            -8.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
        } );
}

SharedPointer< FrameGraphOperation > crimild::framegraph::convolution( std::string name, SharedPointer< FrameGraphResource > const &image, const Matrix3f &kernel ) noexcept
{
    // TODO: move this to a compute pass
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( !name.empty() ? name : "convolution" );

    auto color = useColorAttachment( renderPass->getName() + "/color" );

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
                                CRIMILD_TO_STRING(
                                    layout( location = 0 ) in vec2 inTexCoord;

                                    layout( set = 0, binding = 0 ) uniform ConvolutionUniform {
                                        float kernel[ 9 ];
                                    };

                                    layout( set = 0, binding = 1 ) uniform sampler2D uColorMap;

                                    layout( location = 0 ) out vec4 outColor;

                                    const float offset = 1.0 / 300.0;

                                    void main() {
                                        vec2 offsets[ 9 ] = vec2[](
                                            vec2( -offset, offset ),  // top-left
                                            vec2( 0.0f, offset ),     // top-center
                                            vec2( offset, offset ),   // top-right
                                            vec2( -offset, 0.0 ),     // center-left
                                            vec2( 0.0, 0.0 ),         // center
                                            vec2( offset, 0.0 ),      // center-right
                                            vec2( -offset, -offset ), // bottom-left
                                            vec2( 0.0, -offset ),     // bottom-center
                                            vec2( offset, -offset )   // bottom-right
                                        );

                                        vec3 sampleTex[ 9 ];
                                        for ( int i = 0; i < 9; ++i ) {
                                            sampleTex[ i ] = texture( uColorMap, inTexCoord.st + offsets[ i ] ).rgb;
                                        }

                                        vec3 color = vec3( 0 );
                                        for ( int i = 0; i < 9; ++i ) {
                                            color += sampleTex[ i ] * kernel[ i ];
                                        }

                                        outColor = vec4( color, 1.0 );
                                    } ) ),
                    } );
                program->descriptorSetLayouts = {
                    [] {
                        auto layout = crimild::alloc< DescriptorSetLayout >();
                        layout->bindings = {
                            {
                                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                .stage = Shader::Stage::FRAGMENT,
                            },
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

    auto descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::UNIFORM_BUFFER,
                .obj = [ & ] {
                    struct Uniforms {
                        alignas( 16 ) Matrix3f kernel;
                        //alignas( 16 ) Real32 kernel[ 9 ];
                    };

                    auto data = Uniforms { .kernel = kernel };
                    //memcpy( &data.kernel[ 0 ], kernel.getData(), 9 * sizeof( Real32 ) );
                    return crimild::alloc< UniformBuffer >( data );
                }(),
            },
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), image ),
            },
        };
        return descriptorSet;
    }();

    renderPass->reads( { image } );
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
