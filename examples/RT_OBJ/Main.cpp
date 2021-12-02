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
    virtual void onStarted( void ) noexcept override
    {
        setScene( [ & ] {
            auto scene = crimild::alloc< Group >();

            scene->attachNode( [ & ] {
                auto path = FilePath {
                    .path = "assets/models/bunny/bunny.obj"
                };
                auto group = crimild::alloc< Group >();
                OBJLoader loader( path.getAbsolutePath() );
                loader.setMaterialOverride( "Bunny_Material.001", [] {
                    auto material = crimild::alloc< materials::PrincipledBSDF >();
                    material->setAlbedo( ColorRGB { 0.8, 0.2, 0 } );
                    material->setMetallic( 0.1 );
                    material->setRoughness( 0.5 );
                    return material;
                }() );
                if ( auto model = loader.load() ) {
                    group->attachNode( model );
                }
                return group;
            }() );

            scene->attachNode( crimild::alloc< Skybox >( ColorRGB { 0.5f, 0.6f, 0.7f } ) );

            Simulation::getInstance()->getSettings()->set( "rt.background_color.r", 0.5f );
            Simulation::getInstance()->getSettings()->set( "rt.background_color.g", 0.6f );
            Simulation::getInstance()->getSettings()->set( "rt.background_color.b", 0.7f );

            scene->attachNode(
                [ & ] {
                    auto camera = crimild::alloc< Camera >( 60.0f, 4.0f / 3.0f, 0.1f, 5000.0f );
                    camera->setLocal(
                        lookAt(
                            Point3 { 2, 2, 7 },
                            Point3 { 0, 0, 0 },
                            Vector3 { 0, 1, 0 } ) );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

            scene->perform( StartComponents() );

            return scene;
        }() );

        if ( Simulation::getInstance()->getSettings()->get< std::string >( "video.render_path", "default" ) == "default" ) {
            RenderSystem::getInstance()->useRTSoftRenderPath();
        }
    }
};

CRIMILD_CREATE_SIMULATION( Example, "RT: OBJ File Loader" );
