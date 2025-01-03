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
    virtual Event handle( const Event &e ) noexcept override
    {
        const auto ret = Simulation::handle( e );
        if ( ret.type == Event::Type::SIMULATION_START ) {
            setScene(
                [ & ] {
                    auto scene = crimild::alloc< Group >();

                    scene->attachNode( [ & ] {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive( crimild::alloc< QuadPrimitive >( QuadPrimitive::Params {} ) );
                        geometry->attachComponent< MaterialComponent >(
                            [] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setProgram(
                                    crimild::alloc< ShaderProgram >(
                                        Array< SharedPointer< Shader > > {
                                            crimild::alloc< Shader >(
                                                Shader::Stage::FRAGMENT,
                                                R"(
                                                    void frag_main( inout Fragment frag )
                                                    {
                                                        frag.color = vec3( frag.texCoord, 0.0 );
                                                    }
                                                )"
                                            ),
                                        }
                                    )
                                );
                                return material;
                            }()
                        );
                        return geometry;
                    }() );

                    scene->attachNode( [] {
                        auto camera = crimild::alloc< Camera >();
                        camera->setLocal(
                            lookAt(
                                Point3f { 0, 0, 3 },
                                Point3f { 0, 0, 0 },
                                Vector3::Constants::UP
                            )
                        );
                        return camera;
                    }() );
                    return scene;
                }()
            );
        }
        return ret;
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Texture Coordinates" );
