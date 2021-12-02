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
        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                auto box = []( const Vector3f &position, const RGBAColorf &color ) {
                    auto geometry = crimild::alloc< Geometry >( "box" );
                    geometry->setLayer( Node::Layer::SKYBOX );
                    geometry->attachPrimitive(
                        crimild::alloc< BoxPrimitive >(
                            BoxPrimitive::Params {
                                .type = Primitive::Type::TRIANGLES,
                                .layout = VertexP3N3TC2::getLayout(),
                                .size = Vector3f( 1.0f, 1.0f, 1.0f ),
                            } ) );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ & ] {
                            auto material = crimild::alloc< UnlitMaterial >();
                            material->setColor( color );
                            material->setTexture(
                                [ & ] {
                                    auto texture = crimild::alloc< Texture >();
                                    texture->imageView = [ & ] {
                                        auto imageView = crimild::alloc< ImageView >();
                                        imageView->image = Image::CHECKERBOARD_16;
                                        return imageView;
                                    }();
                                    texture->sampler = [ & ] {
                                        auto sampler = crimild::alloc< Sampler >();
                                        sampler->setMinFilter( Sampler::Filter::NEAREST );
                                        sampler->setMagFilter( Sampler::Filter::NEAREST );
                                        return sampler;
                                    }();
                                    return texture;
                                }() );
                            material->getGraphicsPipeline()->depthStencilState = {
                                .stencilTestEnable = true,
                                .back = {
                                    .compareOp = CompareOp::ALWAYS,
                                    .failOp = StencilOp::REPLACE,
                                    .depthFailOp = StencilOp::REPLACE,
                                    .passOp = StencilOp::REPLACE,
                                    .compareMask = 0xff,
                                    .writeMask = 0xff,
                                    .reference = 1,
                                },
                                .front = {
                                    .compareOp = CompareOp::ALWAYS,
                                    .failOp = StencilOp::REPLACE,
                                    .depthFailOp = StencilOp::REPLACE,
                                    .passOp = StencilOp::REPLACE,
                                    .compareMask = 0xff,
                                    .writeMask = 0xff,
                                    .reference = 1,
                                },
                            };
                            return material;
                        }() );
                    geometry->local().setTranslate( position );
                    geometry->attachComponent< LambdaComponent >(
                        [ position,
                          speed = Random::generate< crimild::Real32 >( 0.5f, 2.0f ) ]( Node *node, const Clock &c ) {
                            auto theta = speed * c.getAccumTime();
                            node->local().setTranslate( position + Numericf::sin( theta ) * Vector3f::UNIT_Y );
                        } );
                    return geometry;
                };

                scene->attachNode( box( Vector3f( 0.0f, 2.0f, 3.0f ), RGBAColorf( 1.0f, 0.5f, 0.31f, 1.0f ) ) );
                scene->attachNode( box( Vector3f( 1.0f, 2.0f, -3.0f ), RGBAColorf( 0.5f, 1.0f, 0.31f, 1.0f ) ) );

                scene->attachNode(
                    [] {
                        auto plane = crimild::alloc< Geometry >();
                        plane->attachPrimitive(
                            crimild::alloc< QuadPrimitive >(
                                QuadPrimitive::Params {} ) );
                        plane->setLocal(
                            [] {
                                Transformation t;
                                t.setTranslate( 0.0f, -1.0f, 0.0f );
                                t.setScale( 10.0f );
                                t.rotate().fromAxisAngle( Vector3f::UNIT_X, -Numericf::HALF_PI );
                                return t;
                            }() );
                        plane->attachComponent< MaterialComponent >()->attachMaterial(
                            [] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setColor( RGBAColorf( 0.31f, 0.5f, 1.0f, 1.0f ) );
                                material->setTexture(
                                    [ & ] {
                                        auto texture = crimild::alloc< Texture >();
                                        texture->imageView = [ & ] {
                                            auto imageView = crimild::alloc< ImageView >();
                                            imageView->image = Image::CHECKERBOARD_16;
                                            return imageView;
                                        }();
                                        texture->sampler = [ & ] {
                                            auto sampler = crimild::alloc< Sampler >();
                                            sampler->setMinFilter( Sampler::Filter::NEAREST );
                                            sampler->setMagFilter( Sampler::Filter::NEAREST );
                                            return sampler;
                                        }();
                                        return texture;
                                    }() );
                                return material;
                            }() );
                        return plane;
                    }() );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->local().setTranslate( 2.0f, 5.0f, 15.0f );
                    camera->local().lookAt( Vector3f::ZERO );
                    Camera::setMainCamera( camera );
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

                        auto outlinePipeline = [ & ] {
                            auto pipeline = cmp.create< GraphicsPipeline >();
                            pipeline->setProgram(
                                [] {
                                    auto program = crimild::alloc< ShaderProgram >(
                                        Array< SharedPointer< Shader > > {
                                            Shader::withBinary(
                                                Shader::Stage::VERTEX,
                                                { .path = "assets/shaders/outline.vert.spv" } ),
                                            Shader::withBinary(
                                                Shader::Stage::FRAGMENT,
                                                { .path = "assets/shaders/outline.frag.spv" } ),
                                        } );
                                    program->vertexLayouts = { VertexP3N3TC2::getLayout() };
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
                            auto &depthStencilState = pipeline->depthStencilState;
                            depthStencilState.stencilTestEnable = true;
                            depthStencilState.back.compareOp = CompareOp::NOT_EQUAL;
                            depthStencilState.back.failOp = StencilOp::KEEP;
                            depthStencilState.back.depthFailOp = StencilOp::KEEP;
                            depthStencilState.back.passOp = StencilOp::REPLACE;
                            depthStencilState.back.compareMask = 0xff;
                            depthStencilState.back.writeMask = 0xff;
                            depthStencilState.back.reference = 1;
                            depthStencilState.front = depthStencilState.back;
                            depthStencilState.depthTestEnable = false;
                            return pipeline;
                        }();

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

                        renderPass->commands = [ & ] {
                            auto commandBuffer = crimild::alloc< CommandBuffer >();
                            Array< Geometry * > boxes;
                            scene->perform(
                                ApplyToGeometries(
                                    [ & ]( Geometry *g ) {
                                        if ( auto ms = g->getComponent< MaterialComponent >() ) {
                                            if ( auto material = ms->first() ) {
                                                commandBuffer->bindGraphicsPipeline( material->getGraphicsPipeline() );
                                                commandBuffer->bindDescriptorSet( renderPass->getDescriptors() );
                                                commandBuffer->bindDescriptorSet( material->getDescriptors() );
                                                commandBuffer->bindDescriptorSet( g->getDescriptors() );
                                                auto p = g->anyPrimitive();
                                                auto vertices = p->getVertexData()[ 0 ];
                                                auto indices = p->getIndices();
                                                commandBuffer->bindVertexBuffer( get_ptr( vertices ) );
                                                commandBuffer->bindIndexBuffer( indices );
                                                commandBuffer->drawIndexed( indices->getIndexCount() );
                                            }
                                        }
                                        if ( g->getName() == "box" ) {
                                            boxes.add( g );
                                        }
                                    } ) );
                            boxes.each(
                                [ & ]( auto geometry ) {
                                    commandBuffer->bindGraphicsPipeline( crimild::get_ptr( outlinePipeline ) );
                                    commandBuffer->bindDescriptorSet( renderPass->getDescriptors() );
                                    commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                                    auto p = geometry->anyPrimitive();
                                    auto vertices = p->getVertexData()[ 0 ];
                                    auto indices = p->getIndices();
                                    commandBuffer->bindVertexBuffer( get_ptr( vertices ) );
                                    commandBuffer->bindIndexBuffer( indices );
                                    commandBuffer->drawIndexed( indices->getIndexCount() );
                                } );
                            return commandBuffer;
                        }();

                        cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

                        return cmp;
                    }() );
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Stencil Outline" );
