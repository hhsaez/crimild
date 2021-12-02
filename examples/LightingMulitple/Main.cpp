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

        setScene( [ & ] {
            auto scene = crimild::alloc< Group >();

            auto box = crimild::alloc< BoxPrimitive >(
                BoxPrimitive::Params {
                    .type = Primitive::Type::TRIANGLES,
                    .layout = VertexP3N3TC2::getLayout(),
                } );

            auto material = crimild::alloc< LitMaterial >();
            material->setMetallic( 0.0f );
            material->setRoughness( 1.0f );

            for ( auto i = 0; i < 100; ++i ) {
                scene->attachNode(
                    [ & ] {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive( box );

                        const auto T = translation(
                            rnd.generate( -10.0f, 10.0f ),
                            rnd.generate( -10.0f, 10.0f ),
                            rnd.generate( -10.0f, 10.0f ) );

                        const auto S = scale( rnd.generate( 0.75f, 1.5f ) );

                        const auto R = rotation(
                            normalize(
                                Vector3 {
                                    Real( rnd.generate( 0.01f, 1.0f ) ),
                                    Real( rnd.generate( 0.01f, 1.0f ) ),
                                    Real( rnd.generate( 0.01f, 1.0f ) ),
                                }
                            ),
                            rnd.generate( 0.0f, Numericf::TWO_PI ) );

                        geometry->setLocal( T * R * S );

                        geometry->attachComponent< MaterialComponent >()->attachMaterial( material );
                        return geometry;
                    }() );
            }

            scene->attachNode(
                [] {
                    auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
                    light->setColor( ColorRGBA { 0.01f, 0.1f, 0.75f, 1 } );
                    return light;
                }() );

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
                                    } ) );
                            geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                [] {
                                    auto material = crimild::alloc< UnlitMaterial >();
                                    material->setColor( ColorRGBA::Constants::GREEN );
                                    return material;
                                }() );
                            return geometry;
                        }() );
                    group->attachNode(
                        [] {
                            auto light = crimild::alloc< Light >( Light::Type::POINT );
                            light->setColor( ColorRGBA { 0.0f, 1.0f, 0.0f, 1.0f } );
                            light->setEnergy( 10.0f );
                            return light;
                        }() );
                    group->attachComponent< LambdaComponent >(
                        []( auto node, auto &clock ) {
                            auto speed = 0.25f;
                            auto t = speed * clock.getCurrentTime();
                            auto x = Numericf::remap( -1.0f, 1.0f, -15.0f, 15.0f, Numericf::cos( t ) * Numericf::sin( t ) );
                            auto y = Numericf::remapSin( -3.0f, 3.0f, t );
                            auto z = Numericf::remapCos( -15.0f, 15.0f, t );
                            node->setLocal( translation( x, y, z ) );
                        } );
                    return group;
                }() );

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
                                    } ) );
                            geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                [] {
                                    auto material = crimild::alloc< UnlitMaterial >();
                                    material->setColor( ColorRGBA::Constants::RED );
                                    return material;
                                }() );
                            return geometry;
                        }() );
                    group->attachNode(
                        [] {
                            auto light = crimild::alloc< Light >( Light::Type::POINT );
                            light->setColor( ColorRGBA::Constants::RED );
                            light->setEnergy( 10.0f );
                            return light;
                        }() );
                    group->attachComponent< LambdaComponent >(
                        []( auto node, auto &clock ) {
                            auto speed = 0.5f;
                            auto t = speed * clock.getCurrentTime();
                            auto x = Numericf::remapSin( -3.0f, 3.0f, t );
                            auto y = Numericf::remap( -1.0f, 1.0f, -15.0f, 15.0f, Numericf::cos( t ) * Numericf::sin( t ) );
                            auto z = Numericf::remapCos( -15.0f, 15.0f, t );
                            node->setLocal( translation( x, y, z ) );
                        } );
                    return group;
                }() );

            scene->attachNode(
                [ & ] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal( translation( 0.0f, 0.0f, 30.0f ) );
                    camera->attachNode(
                        [ & ] {
                            auto light = crimild::alloc< Light >( Light::Type::SPOT );
                            light->setInnerCutoff( Numericf::DEG_TO_RAD * 15.0f );
                            light->setOuterCutoff( Numericf::DEG_TO_RAD * 20.0f );
                            light->setLocal(
                                lookAt(
                                    Point3 { 0, 1, 0 },
                                    Point3 { 0, 0, -5 },
                                    Vector3::Constants::UP
                                )
                            );
                            light->setEnergy( 10.0f );
                            return light;
                        }() );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

            scene->perform( StartComponents() );

            return scene;
        }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Lighting: Multiple Lights" );
