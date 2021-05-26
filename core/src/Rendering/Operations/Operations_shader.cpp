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

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/AssetManager.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::shader( std::string source ) noexcept
{
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( "shader" );

    auto color = useColorAttachment( "shader/color" );

    renderPass->attachments = { color };

    auto descriptors = [ & ] {
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
                            .dimensions = Vector4f { width, height, 0.0f, 0.0f },
                        } );
                }(),
            },
        };
        return descriptorSet;
    }();

    auto prefix = std::string(
        CRIMILD_TO_STRING(
            layout( location = 0 ) in vec2 inTexCoord;

            layout( binding = 0 ) uniform Context {
                vec4 dimensions;
            } context;

            layout( location = 0 ) out vec4 outColor; ) );

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
    }();

    renderPass->writes( { color } );
    renderPass->produces( { color } );

    return withGraphicsCommands(
        renderPass,
        [ pipeline, descriptors ]( auto commandBuffer ) {
            commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
            commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
            commandBuffer->draw( 6 );
        } );
}
