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
        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                auto cylinder = [ primitive = crimild::alloc< Primitive >( Primitive::Type::CYLINDER ) ]( auto material ) -> SharedPointer< Node > {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( primitive );
                    geometry->setLocal( translation( 0, 0, 0 ) );
                    geometry->attachComponent< MaterialComponent >( material );
                    return geometry;
                };

                auto lambertian = []( const auto &albedo ) -> SharedPointer< Material > {
                    auto material = crimild::alloc< materials::PrincipledBSDF >();
                    material->setAlbedo( albedo );
                    return material;
                };

                scene->attachNode(
                    [ & ] {
                        auto c = cylinder( lambertian( ColorRGB { 1, 1, 0 } ) );
                        c->setLocal( scale( 1, 0.25, 1 ) );
                        return c;
                    }() );

                scene->attachNode(
                    [ & ] {
                        auto c = cylinder( lambertian( ColorRGB { 1, 0, 0 } ) );
                        c->setLocal( scale( 0.5, 0.5, 0.5 ) );
                        return c;
                    }() );
                scene->attachNode(
                    [ & ] {
                        auto c = cylinder( lambertian( ColorRGB { 0, 1, 1 } ) );
                        c->setLocal( scale( 0.25, 0.75, 0.25 ) );
                        return c;
                    }() );
                scene->attachNode(
                    [ & ] {
                        auto c = cylinder( lambertian( ColorRGB { 0, 0, 1 } ) );
                        c->setLocal( scale( 0.125, 1, 0.125 ) );
                        return c;
                    }() );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >( 20, 4.0 / 3.0, 0.1f, 1000.0f );
                    camera->setLocal(
                        lookAt(
                            Point3 { 0, 3, 8 },
                            Point3 { 0, 0, 0 },
                            Vector3::Constants::UP ) );
                    camera->setFocusDistance( 10 );
                    camera->setAperture( 0.0f );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                scene->attachNode( [] {
                    auto group = crimild::alloc< Group >();
                    return group;
                }() );

                const auto BACKGROUND_COLOR = ColorRGB { 0.5, 0.6, 0.7 };

                scene->attachNode( crimild::alloc< Skybox >( BACKGROUND_COLOR ) );

                Simulation::getInstance()->getSettings()->set( "rt.background_color.r", BACKGROUND_COLOR.r );
                Simulation::getInstance()->getSettings()->set( "rt.background_color.g", BACKGROUND_COLOR.g );
                Simulation::getInstance()->getSettings()->set( "rt.background_color.b", BACKGROUND_COLOR.b );

                scene->perform( UpdateWorldState() );
                scene->perform( StartComponents() );

                return scene;
            }() );

        // Use soft RT by default
        if ( Simulation::getInstance()->getSettings()->get< std::string >( "video.render_path", "default" ) == "default" ) {
            RenderSystem::getInstance()->useRTSoftRenderPath();
        }
    }
};

CRIMILD_CREATE_SIMULATION( Example, "RT: Cylinders" );
