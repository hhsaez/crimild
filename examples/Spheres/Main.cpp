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

                auto material = crimild::alloc< UnlitMaterial >();
                material->setGraphicsPipeline(
                    [ & ] {
                        auto pipeline = crimild::alloc< GraphicsPipeline >();
                        pipeline->primitiveType = Primitive::Type::TRIANGLES;
                        pipeline->setProgram(
                            [ & ] {
                                auto program = crimild::alloc< UnlitShaderProgram >();
                                program->setShaders(
                                    Array< SharedPointer< Shader > > {
                                        Shader::withSource(
                                            Shader::Stage::VERTEX,
                                            { .path = "assets/shaders/scene.vert" } ),
                                        Shader::withSource(
                                            Shader::Stage::FRAGMENT,
                                            { .path = "assets/shaders/scene.frag" } ),
                                    } );
                                program->vertexLayouts = { VertexLayout::P3_C3 };
                                return program;
                            }() );
                        return pipeline;
                    }() );

                math::fibonacciSquares( 15 ).each(
                    [ & ]( auto &it ) {
                        scene->attachNode(
                            [ & ] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive(
                                    crimild::alloc< SpherePrimitive >(
                                        SpherePrimitive::Params {
                                            .type = Primitive::Type::TRIANGLES,
                                            .layout = VertexP3C3::getLayout(),
                                        } ) );
                                geometry->setLocal( translation( it.first ) * scale( 0.5f * it.second ) );
                                geometry->attachComponent< MaterialComponent >( material );
                                return geometry;
                            }() );
                    } );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal( translation( 0.0f, 0.0f, 250.0f ) );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Spheres" );
