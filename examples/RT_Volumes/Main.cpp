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

namespace crimild {

    SharedPointer< Node > withMaterial( SharedPointer< Node > const &node, SharedPointer< Material > const &material ) noexcept
    {
        node->attachComponent< MaterialComponent >( material );
        return node;
    };

}

using namespace crimild;

class Example : public Simulation {
public:
    void onStarted( void ) noexcept override
    {
        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                auto box = [ primitive = crimild::alloc< Primitive >( Primitive::Type::BOX ) ]() -> SharedPointer< Node > {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( primitive );
                    return geometry;
                };

                auto lambertian = []( const auto &albedo ) -> SharedPointer< Material > {
                    auto material = crimild::alloc< materials::PrincipledBSDF >();
                    material->setAlbedo( albedo );
                    return material;
                };

                auto emissive = []( const auto &color ) -> SharedPointer< Material > {
                    auto material = crimild::alloc< materials::PrincipledBSDF >();
                    material->setEmissive( color );
                    return material;
                };

                auto volume = []( Real density, const auto &color ) -> SharedPointer< Material > {
                    auto material = crimild::alloc< materials::PrincipledVolume >();
                    material->setAlbedo( color );
                    material->setDensity( density );
                    return material;
                };

                scene->attachNode(
                    [ & ] {
                        auto group = crimild::alloc< Group >();
                        Real w = 1.25;
                        Real h = 1.25;
                        Real d = 0.01;
                        group->attachNode( withTranslation( withScale( withMaterial( box(), lambertian( ColorRGB { 1, 1, 1 } ) ), w, d, h ), 0, -h, 0 ) );
                        group->attachNode( withTranslation( withScale( withMaterial( box(), lambertian( ColorRGB { 1, 1, 1 } ) ), w, d, h ), 0, h, 0 ) );
                        group->attachNode( withTranslation( withScale( withMaterial( box(), lambertian( ColorRGB { 1, 1, 1 } ) ), w, h, d ), 0, 0, -h ) );
                        group->attachNode( withTranslation( withScale( withMaterial( box(), lambertian( ColorRGB { 0, 1, 0 } ) ), d, h, h ), w, 0, 0 ) );
                        group->attachNode( withTranslation( withScale( withMaterial( box(), lambertian( ColorRGB { 1, 0, 0 } ) ), d, h, h ), -w, 0, 0 ) );

                        group->attachNode( withTranslation( withScale( withMaterial( box(), emissive( ColorRGB { 10, 10, 10 } ) ), w / 2, d, w / 2 ), 0, h - d, 0 ) );

                        return withScale( withTranslation( group, 0, h, 0 ), 3 );
                    }() );

                scene->attachNode( withTranslation( withRotationY( withScale( withMaterial( box(), volume( 1.0, ColorRGB { 1, 0, 1 } ) ), 1.1 ), 2.5 ), 1.25, 2, 1.25 ) );
                scene->attachNode( withTranslation( withRotationY( withScale( withMaterial( box(), volume( 0.5, ColorRGB { 0, 0, 0 } ) ), 1, 2.5, 1 ), 0.25 ), -1.5, 2.5, -1.5 ) );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal(
                        lookAt(
                            Point3 { 0, 3.5, 15 },
                            Point3 { 0, 3.5, 0 },
                            Vector3::Constants::UP ) );
                    camera->setFocusDistance( 10 );
                    camera->setAperture( 0.0f );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                const auto BACKGROUND_COLOR = ColorRGB { 0.7, 0.6, 0.5 };
                //const auto BACKGROUND_COLOR = ColorRGB { 0, 0, 0 };
                scene->attachNode( crimild::alloc< Skybox >( BACKGROUND_COLOR ) );

                Simulation::getInstance()->getSettings()->set( "rt.background_color.r", BACKGROUND_COLOR.r );
                Simulation::getInstance()->getSettings()->set( "rt.background_color.g", BACKGROUND_COLOR.g );
                Simulation::getInstance()->getSettings()->set( "rt.background_color.b", BACKGROUND_COLOR.b );

                scene->perform( StartComponents() );

                return scene;
            }() );

        // Use soft RT by default
        if ( Simulation::getInstance()->getSettings()->get< std::string >( "video.render_path", "default" ) == "default" ) {
            RenderSystem::getInstance()->useRTSoftRenderPath();
        }
    }
};

CRIMILD_CREATE_SIMULATION( Example, "RT: Volumes" );
