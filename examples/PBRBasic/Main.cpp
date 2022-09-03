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
#include <Crimild_ImGUI.hpp>

using namespace crimild;

class Example : public Simulation {
public:
    virtual Event handle( const Event &e ) noexcept override
    {
        const auto ret = Simulation::handle( e );
        if ( ret.type == Event::Type::SIMULATION_START ) {
            auto rnd = Random::Generator( 1982 );

            setScene( [ & ] {
                auto scene = crimild::alloc< Group >();

                auto primitive = crimild::alloc< SpherePrimitive >(
                    SpherePrimitive::Params {
                        .type = Primitive::Type::TRIANGLES,
                        .layout = VertexP3N3TC2::getLayout(),
                    }
                );

                for ( auto y = 0; y < 7; ++y ) {
                    for ( auto x = 0; x < 7; ++x ) {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive( primitive );
                        geometry->setLocal( translation( 2.5f * Vector3 { -3.0f + x, 3.0f - y, 0 } ) );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [ x, y ] {
                                auto material = crimild::alloc< materials::PrincipledBSDF >();
                                material->setAlbedo( ColorRGB::Constants::WHITE );
                                material->setMetallic( 1.0f - float( y ) / 6.0f );
                                material->setRoughness( float( x ) / 6.0f );
                                return material;
                            }()
                        );
                        scene->attachNode( geometry );
                    }
                }

                auto createLight = [ & ]( const auto &color, auto energy ) {
                    auto group = crimild::alloc< Group >();
                    auto light = [ & ] {
                        auto light = crimild::alloc< Light >( Light::Type::POINT );
                        light->setColor( color );
                        light->setEnergy( energy );
                        return light;
                    }();
                    group->attachNode( light );

                    auto lightPositionIndicator = [ & ] {
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
                            [ & ] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setColor( color );
                                material->setCastShadows( false );
                                return material;
                            }()
                        );
                        return geometry;
                    }();
                    group->attachNode( lightPositionIndicator );

                    auto lightRadiusIndicator = [ & ] {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive(
                            crimild::alloc< SpherePrimitive >(
                                SpherePrimitive::Params {
                                    .type = Primitive::Type::LINES,
                                    .layout = VertexP3N3TC2::getLayout(),
                                    .radius = light->getRadius(),
                                }
                            )
                        );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [ & ] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->getGraphicsPipeline()->primitiveType = Primitive::Type::LINES;
                                material->setColor( color );
                                material->setCastShadows( false );
                                return material;
                            }()
                        );
                        return geometry;
                    }();
                    group->attachNode( lightRadiusIndicator );

                    group->attachComponent< LambdaComponent >(
                        [ maxX = rnd.generate( 5.0, 25.0 ),
                          maxY = rnd.generate( 5.0, 15.0 ),
                          maxZ = rnd.generate( 5.0, 15.0 ),
                          direction = Numericf::sign( rnd.generate( -1.0f, 1.0f ) ),
                          speed = rnd.generate( 0.125f, 0.5f ),
                          lightPositionIndicator,
                          lightRadiusIndicator ]( auto node, auto &clock ) {
                            auto t = Numericf::sign( direction ) * speed * clock.getCurrentTime();
                            auto x = Numericf::remap( -1.0f, 1.0f, -maxX, maxX, Numericf::cos( t ) * Numericf::sin( t ) );
                            auto y = Numericf::remapSin( -maxY, maxY, t );
                            auto z = Numericf::remapCos( -maxZ, maxZ, t );
                            if ( !Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_SPACE ) ) {
                                node->setLocal( translation( x, y, z ) );
                            }

                            auto settings = Simulation::getInstance()->getSettings();
                            auto showLightPositions = settings->get< Bool >( "debug.show_light_positions" );
                            auto showLightVolumes = settings->get< Bool >( "debug.show_light_volumes" );
                            lightPositionIndicator->setEnabled( showLightPositions );
                            lightRadiusIndicator->setEnabled( showLightVolumes );
                        }
                    );
                    return group;
                };

                for ( auto li = 0; li < 30; li++ ) {
                    scene->attachNode(
                        createLight(
                            ColorRGBA {
                                float( rnd.generate( 0.1f, 1.0f ) ),
                                float( rnd.generate( 0.1f, 1.0f ) ),
                                float( rnd.generate( 0.1f, 1.0f ) ),
                                1.0f,
                            },
                            rnd.generate( 10.0f, 50.0f )
                        )
                    );
                }

                scene->attachNode( crimild::alloc< Skybox >( ColorRGB { 0.25f, 0.25f, 0.5f } ) );

                scene->attachNode(
                    [ & ] {
                        auto camera = crimild::alloc< Camera >();
                        camera->setLocal( translation( 0.0f, 0.0f, 30.0f ) );
                        return camera;
                    }()
                );

                scene->perform( StartComponents() );

                return scene;
            }() );
        }
    }
};

CRIMILD_CREATE_SIMULATION( Example, "PBR: Basic" );
