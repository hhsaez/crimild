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
        const auto useRaster = Simulation::getInstance()->getSettings()->get< Bool >( "use_raster", false );
        const auto useCompute = Simulation::getInstance()->getSettings()->get< Bool >( "use_compute", false );

        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                auto sphere = [ & ]( const auto &center, Real radius, auto material ) -> SharedPointer< Node > {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
                    geometry->setLocal( translation( vector3( center ) ) * scale( radius ) );
                    geometry->attachComponent< MaterialComponent >( material );
                    return geometry;
                };

                auto metallic = []( const auto &albedo, auto roughness ) -> SharedPointer< Material > {
                    auto material = crimild::alloc< materials::PrincipledBSDF >();
                    material->setAlbedo( albedo );
                    material->setMetallic( 1 );
                    material->setRoughness( roughness );
                    return material;
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

                auto dielectric = []( auto ior ) -> SharedPointer< Material > {
                    auto material = crimild::alloc< materials::PrincipledBSDF >();
                    material->setTransmission( 1 );
                    material->setIndexOfRefraction( ior );
                    return material;
                };

                Array< SharedPointer< Node > > spheres;

                // Ground
                spheres.add(
                    sphere(
                        Point3 { 0, -1000, 0 },
                        1000,
                        lambertian( ColorRGB { 0.5, 0.5, 0.5 } ) ) );

                spheres.add( sphere( Point3 { 0, 1, 0 }, 1.0, dielectric( 1.5f ) ) );
                spheres.add( sphere( Point3 { 0, 1, 0 }, -0.99, dielectric( 1.5f ) ) );
                spheres.add( sphere( Point3 { -4, 1, 0 }, 1.0, lambertian( ColorRGB { 0.4, 0.2, 0.1 } ) ) );
                spheres.add( sphere( Point3 { 4, 1, 0 }, 1.0f, dielectric( 1.5f ) ) );

                scene->attachNode( framegraph::utils::optimize( spheres ) );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >( 20, 4.0 / 3.0, 0.1f, 1000.0f );
                    camera->setLocal(
                        lookAt(
                            Point3 { 13, 2, 3 },
                            Point3 { 0, 1, -0.5 },
                            Vector3::Constants::UP ) );
                    camera->setFocusDistance( 10 );
                    camera->setAperture( 0.1f );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                scene->attachNode( crimild::alloc< Skybox >( ColorRGB { 0.5f, 0.6f, 0.7f } ) );

                Simulation::getInstance()->getSettings()->set( "rt.background_color.r", 0.5f );
                Simulation::getInstance()->getSettings()->set( "rt.background_color.g", 0.6f );
                Simulation::getInstance()->getSettings()->set( "rt.background_color.b", 0.7f );

                if ( auto settings = Simulation::getInstance()->getSettings() ) {
                    if ( settings->hasKey( "rt.hd" ) ) {
                        settings->set( "rt.width", 1200 );
                        settings->set( "rt.height", 800 );
                        settings->set( "rt.samples", 500 );
                        settings->set( "rt.depth", 50 );
                    }
                }

                scene->perform( UpdateWorldState() );
                scene->perform( StartComponents() );

                return scene;
            }() );

        if ( Simulation::getInstance()->getSettings()->get< std::string >( "video.render_path", "default" ) == "default" ) {
            RenderSystem::getInstance()->useRTSoftRenderPath();
        }
    }
};

CRIMILD_CREATE_SIMULATION( Example, "RT: Glass" );
