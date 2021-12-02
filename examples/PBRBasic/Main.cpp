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
    void onStarted( void ) noexcept override
    {
        setScene( [ & ] {
            auto scene = crimild::alloc< Group >();

            auto primitive = crimild::alloc< SpherePrimitive >(
                SpherePrimitive::Params {
                    .type = Primitive::Type::TRIANGLES,
                    .layout = VertexP3N3TC2::getLayout(),
                } );

            for ( auto y = 0; y < 7; ++y ) {
                for ( auto x = 0; x < 7; ++x ) {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( primitive );
                    geometry->local().setTranslate( 2.5f * Vector3f( -3.0f + x, 3.0f - y, 0 ) );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ x, y ] {
                            auto material = crimild::alloc< LitMaterial >();
                            material->setAlbedo( RGBColorf::ONE );
                            material->setMetallic( 1.0f - float( y ) / 6.0f );
                            material->setRoughness( float( x ) / 6.0f );
                            return material;
                        }() );
                    scene->attachNode( geometry );
                }
            }

            auto createLight = []( const auto &color, auto energy ) {
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
                            } ) );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ & ] {
                            auto material = crimild::alloc< UnlitMaterial >();
                            material->setColor( color );
                            material->setCastShadows( false );
                            return material;
                        }() );
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
                            } ) );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ & ] {
                            auto material = crimild::alloc< UnlitMaterial >();
                            material->getGraphicsPipeline()->primitiveType = Primitive::Type::LINES;
                            material->setColor( color );
                            material->setCastShadows( false );
                            return material;
                        }() );
                    return geometry;
                }();
                group->attachNode( lightRadiusIndicator );

                group->attachComponent< LambdaComponent >(
                    [ maxX = Random::generate( 5.0, 25.0 ),
                      maxY = Random::generate( 5.0, 15.0 ),
                      maxZ = Random::generate( 5.0, 15.0 ),
                      direction = Numericf::sign( Random::generate( -1.0f, 1.0f ) ),
                      speed = Random::generate( 0.125f, 0.5f ),
                      lightPositionIndicator,
                      lightRadiusIndicator ]( auto node, auto &clock ) {
                        auto t = Numericf::sign( direction ) * speed * clock.getCurrentTime();
                        auto x = Numericf::remap( -1.0f, 1.0f, -maxX, maxX, Numericf::cos( t ) * Numericf::sin( t ) );
                        auto y = Numericf::remapSin( -maxY, maxY, t );
                        auto z = Numericf::remapCos( -maxZ, maxZ, t );
                        if ( !Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_SPACE ) ) {
                            node->local().setTranslate( x, y, z );
                        }

                        auto settings = Simulation::getInstance()->getSettings();
                        auto showLightPositions = settings->get< Bool >( "debug.show_light_positions" );
                        auto showLightVolumes = settings->get< Bool >( "debug.show_light_volumes" );
                        lightPositionIndicator->setEnabled( showLightPositions );
                        lightRadiusIndicator->setEnabled( showLightVolumes );
                    } );
                return group;
            };

            for ( auto li = 0; li < 30; li++ ) {
                scene->attachNode(
                    createLight(
                        RGBAColorf(
                            Random::generate( 0.1f, 1.0f ),
                            Random::generate( 0.1f, 1.0f ),
                            Random::generate( 0.1f, 1.0f ),
                            1.0f ),
                        Random::generate( 10.0f, 50.0f ) ) );
            }

            scene->attachNode( crimild::alloc< Skybox >( RGBColorf( 0.25f, 0.25f, 0.5f ) ) );

            scene->attachNode(
                [ & ] {
                    auto camera = crimild::alloc< Camera >();
                    camera->local().setTranslate( 0.0f, 0.0f, 30.0f );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

            scene->perform( StartComponents() );

            return scene;
        }() );

        imgui::ImGUISystem::getInstance()->setFrameCallback(
            [ open = true,
              showLightPositions = true,
              showLightVolumes = false ]() mutable {
                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_ESCAPE ) ) {
                    open = !open;
                }

                if ( !open ) {
                    return;
                }

                {
                    ImGui::Begin( "Settings" );
                    ImGui::Checkbox( "Positions", &showLightPositions );
                    ImGui::Checkbox( "Volumes", &showLightVolumes );
                    ImGui::End();
                }

                auto settings = Simulation::getInstance()->getSettings();
                settings->set( "debug.show_light_positions", showLightPositions );
                settings->set( "debug.show_light_volumes", showLightVolumes );
            } );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "PBR: Basic" );
