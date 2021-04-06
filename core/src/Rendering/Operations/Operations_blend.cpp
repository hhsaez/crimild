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

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::blend( Array< SharedPointer< FrameGraphResource > > const &resources ) noexcept
{
    // TODO: move this to a compute pass
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "blend" );

    auto format = Format::R8G8B8A8_UNORM;
    if ( !resources.empty() ) {
        format = useFormat( resources[ 0 ] );
    }

    auto color = useColorAttachment( "blend/color", format );

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
                                layout ( location = 0 ) in vec2 inTexCoord;

                                layout ( set = 0, binding = 0 ) uniform sampler2D uColorMap;

                                layout ( location = 0 ) out vec4 outColor;

                                void main()
                                {
                                    outColor = texture( uColorMap, inTexCoord );
                                }
                            )" ),
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
        pipeline->depthStencilState.depthTestEnable = false;
        pipeline->colorBlendState = ColorBlendState {
            .enable = true,
            .srcColorBlendFactor = BlendFactor::ONE, //DST_COLOR,
            .dstColorBlendFactor = BlendFactor::ONE, //ZERO,
        };
        return pipeline;
    }();

    auto descriptors = resources.map(
        []( auto resource ) {
            auto descriptorSet = crimild::alloc< DescriptorSet >();
            descriptorSet->descriptors = {
                Descriptor {
                    .descriptorType = DescriptorType::TEXTURE,
                    .obj = withResource( crimild::alloc< Texture >(), resource ),
                },
            };
            return descriptorSet;
        } );

    renderPass->reads( resources );
    renderPass->writes( { color } );
    renderPass->produces( { color } );

    return withGraphicsCommands(
        renderPass,
        [ pipeline, descriptors ]( auto commandBuffer ) {
            descriptors.each(
                [ & ]( auto descriptorSet ) {
                    commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
                    commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptorSet ) );
                    commandBuffer->draw( 6 );
                } );
        } );
}
