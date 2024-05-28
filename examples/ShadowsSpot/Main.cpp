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
            setScene( [ & ] {
                auto scene = crimild::alloc< Group >();

                scene->attachNode(
                    [] {
                        auto primitive = crimild::alloc< BoxPrimitive >(
                            BoxPrimitive::Params {
                                .type = Primitive::Type::TRIANGLES,
                                .layout = VertexP3N3TC2::getLayout(),
                            }
                        );

                        auto material = [] {
                            auto material = crimild::alloc< materials::PrincipledBSDF >();
                            material->setAlbedo( ColorRGB { 0.0f, 1.0f, 0.0f } );
                            material->setMetallic( 0.0f );
                            material->setRoughness( 1.0f );
                            return material;
                        }();

                        auto group = crimild::alloc< Group >();
                        auto rnd = Random::Generator( 1982 );
                        for ( auto i = 0; i < 30; ++i ) {
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive( primitive );

                                    geometry->setLocal(
                                        translation(
                                            rnd.generate( -10.0f, 10.0f ),
                                            rnd.generate( -10.0f, 10.0f ),
                                            rnd.generate( -10.0f, 10.0f )
                                        )
                                    );

                                    geometry->attachComponent< LambdaComponent >(
                                        [ origin = origin( geometry->getLocal() ),
                                          speed = ( rnd.generate( -1.0f, 1.0f ) < 0 ? -1.0f : 1.0f ) * rnd.generate( 1.0f, 6.0f ) ]( auto node, const auto &clock ) {
                                            node->setLocal( translation( Vector3f( origin + Vector3f::Constants::UNIT_Y * speed * Numericf::sin( clock.getCurrentTime() ) ) ) );
                                        }
                                    );

                                    geometry->attachComponent< MaterialComponent >()->attachMaterial( material );

                                    return geometry;
                                }()
                            );
                        }
                        return group;
                    }()
                );

                scene->attachNode(
                    [] {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive(
                            crimild::alloc< QuadPrimitive >(
                                QuadPrimitive::Params {}
                            )
                        );
                        geometry->setLocal(
                            [] {
                                const auto R = rotationX( -Numericf::HALF_PI );
                                const auto S = scale( 100.0f );
                                const auto T = translation( 0.0f, -15.0f, 0.0f );
                                return T( R( S ) );
                            }()
                        );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [] {
                                auto material = crimild::alloc< materials::PrincipledBSDF >();
                                material->setMetallic( 0.0f );
                                material->setRoughness( 1.0f );
                                return material;
                            }()
                        );
                        return geometry;
                    }()
                );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal(
                        lookAt(
                            Point3f { 15, 20, 50 },
                            Point3f { 0, 1, 0 },
                            Vector3::Constants::UP
                        )
                    );
                    return camera;
                }() );

                scene->attachNode( crimild::alloc< Skybox >( ColorRGB { 0.002f, 0.001f, 0.002f } ) );

                scene->attachNode(
                    [ & ] {
                        auto light = crimild::alloc< Group >();

                        light->attachNode(
                            [] {
                                auto group = crimild::alloc< Group >();

                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setColor( ColorRGBA::Constants::WHITE );

                                auto primitive = crimild::alloc< ArrowPrimitive >(
                                    ArrowPrimitive::Params {
                                        .type = Primitive::Type::TRIANGLES,
                                        .layout = VertexP3N3TC2::getLayout(),
                                    }
                                );

                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive( primitive );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial( material );
                                group->attachNode( geometry );

                                return group;
                            }()
                        );

                        light->attachNode(
                            [] {
                                auto light = crimild::alloc< Light >( Light::Type::SPOT );
                                light->setCastShadows( true );
                                light->setColor( ColorRGB::Constants::WHITE );
                                light->setEnergy( 1000.0f );
                                light->setInnerCutoff( Numericf::DEG_TO_RAD * 25.0f );
                                light->setOuterCutoff( Numericf::DEG_TO_RAD * 40.0f );
                                return light;
                            }()
                        );

                        light->setLocal(
                            lookAt(
                                Point3f { 20.0f, 10.0f, 40.0f },
                                Point3f { 0, 0, 0 },
                                Vector3::Constants::UP
                            )
                        );

                        auto pivot = crimild::alloc< Group >();
                        pivot->attachNode( light );
                        return pivot;
                    }()
                );

                scene->perform( StartComponents() );

                return scene;
            }() );
        }

        return ret;
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Shadows: Spot" );
