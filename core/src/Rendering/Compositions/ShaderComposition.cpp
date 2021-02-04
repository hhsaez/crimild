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

#include "Rendering/Compositions/ShaderComposition.hpp"

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::compositions;

Composition crimild::compositions::shader( const std::string &source ) noexcept
{
    Composition cmp;
    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [ & ] {
            auto att = cmp.createAttachment( "shader" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            att->format = Format::R8G8B8A8_UNORM;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }(),
    };

    auto prefix = std::string(
        CRIMILD_TO_STRING(
            layout( location = 0 ) in vec2 inTexCoord;

            layout( binding = 0 ) uniform Context {
                vec4 dimensions;
            } context;

            layout( location = 0 ) out vec4 outColor; ) );

    renderPass->setGraphicsPipeline(
        [ & ] {
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
                                    vec2( 0.0, 1.0 ),
                                    vec2( 0.0, 0.0 ),
                                    vec2( 1.0, 0.0 ),

                                    vec2( 0.0, 1.0 ),
                                    vec2( 1.0, 0.0 ),
                                    vec2( 1.0, 1.0 )
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
                                prefix + source ),
                        } );
                    program->descriptorSetLayouts = {
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
            return pipeline;
        }() );

    renderPass->setDescriptors(
        [ & ] {
            auto descriptorSet = crimild::alloc< DescriptorSet >();
            descriptorSet->descriptors = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = [ & ] {
                        struct ContextDescriptor {
                            Vector4f dimensions;
                        };

                        auto settings = Simulation::getInstance()->getSettings();
                        auto width = settings->get< float >( "video.width", 1024 );
                        auto height = settings->get< float >( "video.height", 768 );
                        return crimild::alloc< UniformBuffer >(
                            ContextDescriptor {
                                .dimensions = Vector4f( width, height, 0.0f, 0.0f ),
                            } );
                    }(),
                },
            };
            return descriptorSet;
        }() );

    auto commandBuffer = cmp.create< CommandBuffer >();
    commandBuffer->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
    commandBuffer->beginRenderPass( renderPass, nullptr );
    commandBuffer->bindGraphicsPipeline( renderPass->getGraphicsPipeline() );
    commandBuffer->bindDescriptorSet( renderPass->getDescriptors() );
    commandBuffer->draw( 6 );
    commandBuffer->endRenderPass( renderPass );
    commandBuffer->end();

    renderPass->setCommandRecorder(
        [ commandBuffer ]() {
            return commandBuffer;
        }
    );

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
