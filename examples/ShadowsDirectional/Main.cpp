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
        setScene( [ & ] {
            auto scene = crimild::alloc< Group >();

            scene->attachNode(
                [] {
                    auto primitive = crimild::alloc< BoxPrimitive >(
                        BoxPrimitive::Params {
                            .type = Primitive::Type::TRIANGLES,
                            .layout = VertexP3N3TC2::getLayout(),
                        } );

                    auto material = [] {
                        auto material = crimild::alloc< LitMaterial >();
                        material->setMetallic( 0.0f );
                        material->setRoughness( 1.0f );
                        material->setAlbedo( ColorRGB { 250.0 / 255.0, 128.0 / 255.0, 114.0 / 255.0 } );
                        return material;
                    }();

                    auto group = crimild::alloc< Group >();
                    for ( auto i = 0; i < 30; ++i ) {
                        group->attachNode(
                            [ & ] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive( primitive );

                                const auto T = translation( 30 - 2 * i, 0, 100 - 7 * i );
                                const auto S = scale( 1, 20, 1 );
                                geometry->setLocal( T * S );

                                geometry->attachComponent< MaterialComponent >()->attachMaterial( material );

                                return geometry;
                            }() );
                    }
                    return group;
                }() );

            scene->attachNode(
                [] {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive(
                        crimild::alloc< QuadPrimitive >(
                            QuadPrimitive::Params {} ) );
                    geometry->setLocal(
                        [] {
                            const auto R = rotationX( -Numericf::HALF_PI );
                            const auto S = scale( 100.0f );
                            const auto T = translation( 0.0f, -15.0f, 0.0f );
                            return T * R * S;
                        }() );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [] {
                            auto material = crimild::alloc< LitMaterial >();
                            material->setMetallic( 0.0f );
                            material->setRoughness( 1.0f );
                            return material;
                        }() );
                    return geometry;
                }() );

            scene->attachNode( [] {
                auto camera = crimild::alloc< Camera >( 60, 4.0f / 3.0f, 1.0f, 500.0f );
                camera->setLocal( translation( 15.0f, 20.0f, 50.0f ) );
                camera->attachComponent< FreeLookCameraComponent >();
                return camera;
            }() );

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
                                } );

                            auto rnd = Random::Generator( 1999 );
                            for ( auto i = 0; i < 3; i++ ) {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive( primitive );
                                geometry->setLocal(
                                    translation(
                                        rnd.generate( -2.0f, 2.0f ),
                                        rnd.generate( -2.0f, 2.0f ),
                                        rnd.generate( -1.0f, 1.0f ) ) );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial( material );
                                group->attachNode( geometry );
                            }

                            return group;
                        }() );

                    light->attachNode(
                        [] {
                            auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
                            light->setCastShadows( true );
                            return light;
                        }() );

                    light->setLocal(
                        lookAt(
                            Point3 { 20, 20, 20 },
                            Point3 { 0, 0, 0 },
                            Vector3::Constants::UP ) );

                    auto pivot = crimild::alloc< Group >();
                    pivot->attachNode( light );
                    return pivot;
                }() );

            scene->perform( StartComponents() );

            return scene;
        }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Shadows: Directional" );
