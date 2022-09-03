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
                                            .layout = VertexP3N3TC2::getLayout(),
                                        }
                                    )
                                );

                                const auto T = translation(
                                    rnd.generate( -10.0f, 10.0f ),
                                    rnd.generate( -10.0f, 10.0f ),
                                    rnd.generate( -10.0f, 10.0f )
                                );
                                const auto S = scale( rnd.generate( 0.75f, 1.5f ) );
                                geometry->setLocal( T * S );

                                geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                    [ & ] {
                                        auto material = crimild::alloc< materials::PrincipledBSDF >();
                                        material->setAlbedo( ColorRGB { 0.0f, 1.0f, 0.0f } );
                                        material->setMetallic( 0.0f );
                                        material->setRoughness( 1.0f );
                                        return material;
                                    }()
                                );
                                return geometry;
                            }()
                        );
                    }

                    scene->attachNode(
                        [] {
                            auto group = crimild::alloc< Group >();
                            group->attachNode(
                                [] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< ArrowPrimitive >(
                                            ArrowPrimitive::Params {
                                                .type = Primitive::Type::TRIANGLES,
                                                .layout = VertexP3N3TC2::getLayout(),
                                            }
                                        )
                                    );
                                    constexpr auto T = translation( 35.0f * Vector3::Constants::UNIT_Z );
                                    constexpr auto S = scale( 3.0f );
                                    geometry->setLocal( T * S );
                                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                        [] {
                                            auto material = crimild::alloc< UnlitMaterial >();
                                            material->setColor( ColorRGBA::Constants::WHITE );
                                            return material;
                                        }()
                                    );
                                    return geometry;
                                }()
                            );
                            group->attachNode(
                                [] {
                                    auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
                                    light->setColor( ColorRGB::Constants::WHITE );
                                    light->setEnergy( 1.0f );
                                    return light;
                                }()
                            );
                            return group;
                        }()
                    );

                    scene->attachNode(
                        [ & ] {
                            auto camera = crimild::alloc< Camera >();
                            camera->setLocal( translation( 0.0f, 0.0f, 30.0f ) );
                            camera->attachComponent< FreeLookCameraComponent >();
                            return camera;
                        }()
                    );

                    scene->perform( StartComponents() );

                    return scene;
                }()
            );
        }
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Lighting: Directional Lights" );
