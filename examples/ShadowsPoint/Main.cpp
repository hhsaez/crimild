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
                        SharedPointer< Primitive > primitives[ 6 ] = {
                            crimild::alloc< BoxPrimitive >(
                                BoxPrimitive::Params {
                                    .type = Primitive::Type::TRIANGLES,
                                    .layout = VertexP3N3TC2::getLayout(),
                                }
                            ),
                            crimild::alloc< SpherePrimitive >(
                                SpherePrimitive::Params {
                                    .type = Primitive::Type::TRIANGLES,
                                    .layout = VertexP3N3TC2::getLayout(),
                                }
                            ),
                            crimild::alloc< TorusPrimitive >(
                                TorusPrimitive::Params {
                                    .type = Primitive::Type::TRIANGLES,
                                    .layout = VertexP3N3TC2::getLayout(),
                                }
                            ),
                            crimild::alloc< ConePrimitive >(
                                ConePrimitive::Params {
                                    .type = Primitive::Type::TRIANGLES,
                                    .layout = VertexP3N3TC2::getLayout(),
                                }
                            ),
                            crimild::alloc< CylinderPrimitive >(
                                CylinderPrimitive::Params {
                                    .type = Primitive::Type::TRIANGLES,
                                    .layout = VertexP3N3TC2::getLayout(),
                                }
                            ),
                            crimild::alloc< TrefoilKnotPrimitive >(
                                TrefoilKnotPrimitive::Params {
                                    .type = Primitive::Type::TRIANGLES,
                                    .layout = VertexP3N3TC2::getLayout(),
                                }
                            ),
                        };

                        auto material = [] {
                            auto material = crimild::alloc< materials::PrincipledBSDF >();
                            material->setAlbedo( ColorRGB { 0.0f, 1.0f, 0.0f } );
                            material->setMetallic( 0.0f );
                            material->setRoughness( 1.0f );
                            return material;
                        }();

                        auto group = crimild::alloc< Group >();
                        auto rnd = Random::Generator( 1982 );
                        for ( auto i = 0; i < 6; ++i ) {
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive( primitives[ i % 6 ] );

                                    //                                const auto T = translation(
                                    //                                    rnd.generate( -15.0f, 15.0f ),
                                    //                                    rnd.generate( -15.0f, 15.0f ),
                                    //                                    rnd.generate( -15.0f, 15.0f ) );
                                    const auto pos = Array< Vector3 > {
                                        { -5, 0, 0 },
                                        { 5, 0, 0 },
                                        { 0, -5, 0 },
                                        { 0, 5, 0 },
                                        { 0, 0, -5 },
                                        { 0, 0, 5 },
                                    };
                                    const auto T = translation( pos[ i % 6 ] );

                                    const auto S = scale( rnd.generate( 1.75f, 1.5f ) );

                                    const auto R = rotation(
                                        normalize(
                                            Vector3 {
                                                Real( rnd.generate( 0.01f, 1.0f ) ),
                                                Real( rnd.generate( 0.01f, 1.0f ) ),
                                                Real( rnd.generate( 0.01f, 1.0f ) ),
                                            }
                                        ),
                                        rnd.generate( 0.0f, Numericf::TWO_PI )
                                    );

                                    geometry->setLocal( T * R * S );

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
                        auto primitive = crimild::alloc< BoxPrimitive >(
                            BoxPrimitive::Params {
                                .type = Primitive::Type::TRIANGLES,
                                .layout = VertexP3N3TC2::getLayout(),
                                .size = 20.0f * Vector3::Constants::ONE,
                                .invertFaces = true,
                            }
                        );

                        auto material = [] {
                            auto material = crimild::alloc< materials::PrincipledBSDF >();
                            material->setAlbedo( ColorRGB::Constants::WHITE );
                            material->setMetallic( 0.0f );
                            material->setRoughness( 1.0f );
                            return material;
                        }();

                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive( primitive );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial( material );
                        return geometry;
                    }()
                );

                scene->attachNode(
                    [] {
                        auto group = crimild::alloc< Group >();
                        group->attachNode(
                            [] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive(
                                    crimild::alloc< SpherePrimitive >(
                                        SpherePrimitive::Params {
                                            .type = Primitive::Type::TRIANGLES,
                                            .layout = VertexP3N3TC2::getLayout(),
                                            .radius = 0.1f,
                                        }
                                    )
                                );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                    [] {
                                        auto material = crimild::alloc< UnlitMaterial >();
                                        material->setColor( ColorRGBA::Constants::WHITE );
                                        material->setCastShadows( false );
                                        return material;
                                    }()
                                );
                                return geometry;
                            }()
                        );
                        group->attachNode(
                            [] {
                                auto light = crimild::alloc< Light >( Light::Type::POINT );
                                light->setColor( ColorRGB::Constants::WHITE );
                                light->setEnergy( 100.0f );
                                light->setCastShadows( true );
                                return light;
                            }()
                        );
                        group->attachComponent< LambdaComponent >(
                            []( auto node, auto &clock ) {
                                auto speed = 0.125f;
                                auto t = speed * clock.getCurrentTime();
                                auto x = Numericf::remap( -1.0f, 1.0f, -15.0f, 15.0f, Numericf::cos( t ) * Numericf::sin( t ) );
                                auto y = Numericf::remapSin( -3.0f, 3.0f, t );
                                auto z = Numericf::remapCos( -15.0f, 15.0f, t );
                                if ( !Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_SPACE ) ) {
                                    // node->setLocal( translation( x, y, z ) );
                                }

                                auto dir = Vector3::Constants::ZERO;
                                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_I ) ) {
                                    dir = Vector3::Constants::FORWARD;
                                }
                                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_K ) ) {
                                    dir = -Vector3::Constants::FORWARD;
                                }
                                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_L ) ) {
                                    dir = Vector3::Constants::RIGHT;
                                }
                                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_J ) ) {
                                    dir = -Vector3::Constants::RIGHT;
                                }
                                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_Y ) ) {
                                    dir = Vector3::Constants::UP;
                                }
                                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_H ) ) {
                                    dir = -Vector3::Constants::UP;
                                }

                                const auto p = location( node->getLocal() );
                                const auto dp = 2.0f * clock.getDeltaTime() * dir;
                                node->setLocal( translation( vector3( p + dp ) ) );
                            }
                        );
                        return group;
                    }()
                );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal(
                        lookAt(
                            Point3 { 15.0f, 5.0f, 40.0f },
                            Point3 { 0, 1, 0 },
                            Vector3::Constants::UP
                        )
                    );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
        }
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Shadows: Point" );
