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
        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                scene->attachNode( crimild::alloc< Skybox >( RGBColorf( 0.1f, 0.05f, 0.5f ) ) );

                scene->attachNode(
                    [] {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive(
                            crimild::alloc< QuadPrimitive >(
                                QuadPrimitive::Params {
                                    .layout = VertexP3N3TC2::getLayout(),
                                } ) );
                        geometry->local().rotate().fromAxisAngle( Vector3f::UNIT_X, -Numericf::HALF_PI );
                        geometry->local().setScale( 10.0f );
                        geometry->attachComponent< MaterialComponent >(
                            [] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setColor( RGBAColorf( 1.0f, 0.6f, 0.15f, 1.0f ) );
                                return material;
                            }() );
                        return geometry;
                    }() );

                scene->attachNode(
                    [] {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive(
                            crimild::alloc< BoxPrimitive >(
                                BoxPrimitive::Params {
                                    .layout = VertexP3N3TC2::getLayout(),
                                } ) );
                        geometry->local().setTranslate( 0.0f, 1.0f, 0.0f );
                        geometry->attachComponent< MaterialComponent >(
                            [] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setColor( RGBAColorf( 0.5f, 1.0f, 0.0f, 1.0f ) );
                                return material;
                            }() );
                        return geometry;
                    }() );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->local().setTranslate( 3.0f, 4.0f, 10.0f );
                    camera->local().lookAt( 0.5f * Vector3f::UNIT_Y );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                scene->perform( StartComponents() );
                return scene;
            }() );

        imgui::ImGUISystem::getInstance()->setFrameCallback(
            [] {
                // Overrides default UI. Just for demo purposes
                static bool open = true;
                ImGui::ShowDemoWindow( &open );

                {
                    auto s = Simulation::getInstance()->getSimulationClock().getDeltaTime();

                    ImGui::Begin( "Stats" );
                    ImGui::Text( "Frame Time: %.2f ms", 1000.0f * s );
                    ImGui::Text( "FPS: %d", s > 0 ? int( 1.0 / s ) : 0 );
                    ImGui::End();
                }
            } );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "ImGUI: Basics" );