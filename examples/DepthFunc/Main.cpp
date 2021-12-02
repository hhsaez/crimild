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

#include <Crimild.hpp>

using namespace crimild;

class Example : public Simulation {
public:
    void onStarted( void ) noexcept override
    {
        auto rnd = Random::Generator( 1982 );

        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                for ( auto i = 0; i < 30; ++i ) {
                    scene->attachNode(
                        [ & ] {
                            auto geometry = crimild::alloc< Geometry >();
                            geometry->attachPrimitive(
                                crimild::alloc< BoxPrimitive >(
                                    BoxPrimitive::Params {
                                        .type = Primitive::Type::TRIANGLES,
                                        .layout = VertexP3::getLayout(),
                                    } ) );

                            geometry->local().setTranslate(
                                rnd.generate( -10.0f, 10.0f ),
                                rnd.generate( -10.0f, 10.0f ),
                                rnd.generate( -10.0f, 10.0f ) );

                            geometry->local().setScale( rnd.generate( 0.75f, 1.5f ) );

                            geometry->local().rotate().fromAxisAngle(
                                Vector3f(
                                    rnd.generate( 0.01f, 1.0f ),
                                    rnd.generate( 0.01f, 1.0f ),
                                    rnd.generate( 0.01f, 1.0f ) )
                                    .getNormalized(),
                                rnd.generate( 0.0f, Numericf::TWO_PI ) );

                            return geometry;
                        }() );
                }

                scene->attachNode(
                    [ & ] {
                        auto camera = crimild::alloc< Camera >();
                        camera->local().setTranslate( 0.0f, 0.0f, 30.0f );
                        return camera;
                    }() );

                scene->perform( StartComponents() );

                return scene;
            }() );

        setComposition(
            [ scene = getScene() ] {
                using namespace crimild::compositions;
                return present(
                    [ scene ] {
                        Composition cmp;

                        auto renderPass = cmp.create< RenderPass >();
                        renderPass->attachments = {
                            [ & ] {
                                auto att = crimild::alloc< Attachment >();
                                att->format = Format::R8G8B8A8_UNORM;
                                att->imageView = crimild::alloc< ImageView >();
                                att->imageView->image = crimild::alloc< Image >();
                                return att;
                            }(),
                            [ & ] {
                                auto att = crimild::alloc< Attachment >();
                                att->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
                                att->imageView = crimild::alloc< ImageView >();
                                att->imageView->image = crimild::alloc< Image >();
                                return att;
                            }()
                        };

                        renderPass->setDescriptors(
                            [ & ] {
                                auto descriptorSet = crimild::alloc< DescriptorSet >();
                                descriptorSet->descriptors = {
                                    Descriptor {
                                        .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                        .obj = [ & ] {
                                            FetchCameras fetch;
                                            scene->perform( fetch );
                                            auto camera = fetch.anyCamera();
                                            return crimild::alloc< CameraViewProjectionUniform >( camera );
                                        }(),
                                    },
                                };
                                return descriptorSet;
                            }() );

                        renderPass->setGraphicsPipeline(
                            [] {
                                auto pipeline = crimild::alloc< GraphicsPipeline >();
                                pipeline->setProgram(
                                    [] {
                                        auto program = crimild::alloc< ShaderProgram >(
                                            Array< SharedPointer< Shader > > {
                                                Shader::withBinary(
                                                    Shader::Stage::VERTEX,
                                                    { .path = "assets/shaders/depth.vert.spv" } ),
                                                Shader::withBinary(
                                                    Shader::Stage::FRAGMENT,
                                                    { .path = "assets/shaders/depth.frag.spv" } ),
                                            } );
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
                                                        .descriptorType = DescriptorType::UNIFORM_BUFFER,
                                                        .stage = Shader::Stage::VERTEX,
                                                    },
                                                };
                                                return layout;
                                            }(),
                                        };
                                        return program;
                                    }() );
                                pipeline->depthStencilState.depthTestEnable = false;
                                return pipeline;
                            }() );

                        renderPass->commands = [ & ] {
                            auto commandBuffer = crimild::alloc< CommandBuffer >();
                            scene->perform(
                                ApplyToGeometries(
                                    [ & ]( Geometry *g ) {
                                        commandBuffer->bindGraphicsPipeline( renderPass->getGraphicsPipeline() );
                                        commandBuffer->bindDescriptorSet( renderPass->getDescriptors() );
                                        commandBuffer->bindDescriptorSet( g->getDescriptors() );
                                        commandBuffer->drawPrimitive( g->anyPrimitive() );
                                    } ) );
                            return commandBuffer;
                        }();

                        cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

                        return cmp;
                    }() );
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Depth Funct" );
