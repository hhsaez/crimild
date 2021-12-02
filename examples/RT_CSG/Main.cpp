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

                auto sphere = [ primitive = crimild::alloc< Primitive >( Primitive::Type::SPHERE ) ]( const auto &center, Real radius, auto material ) -> SharedPointer< Node > {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( primitive );
                    geometry->setLocal( translation( vector3( center ) ) * scale( radius ) );
                    geometry->attachComponent< MaterialComponent >( material );
                    return geometry;
                };

                auto box = [ primitive = crimild::alloc< Primitive >( Primitive::Type::BOX ) ]( const auto &center, const auto &size, auto material ) -> SharedPointer< Node > {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( primitive );
                    geometry->setLocal( translation( vector3( center ) ) * scale( size.x, size.y, size.z ) );
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

                // Ground
                scene->attachNode(
                    sphere(
                        Point3 { 0, -1000, 0 },
                        1000,
                        lambertian( ColorRGB { 0.5, 0.5, 0.5 } ) ) );

                scene->attachNode(
                    crimild::alloc< CSGNode >(
                        CSGNode::Operator::UNION,
                        box( Point3 { 0, 1, 0 }, Vector3 { 1, 1, 1 }, lambertian( ColorRGB { 0.9, 0.9, 0.1 } ) ),
                        sphere( Point3 { 1, 2, 1 }, 1.0, metallic( ColorRGB { 0.9, 0.2, 0.1 }, 0.1 ) ) ) );

                scene->attachNode(
                    crimild::alloc< CSGNode >(
                        CSGNode::Operator::INTERSECTION,
                        sphere( Point3 { -3, 2, 1 }, 1.0, lambertian( ColorRGB { 0.6, 0.2, 0.1 } ) ),
                        box( Point3 { -4, 1, 0 }, Vector3 { 1, 1, 1 }, lambertian( ColorRGB { 0.4, 0.4, 0.1 } ) ) ) );

                scene->attachNode(
                    crimild::alloc< CSGNode >(
                        CSGNode::Operator::DIFFERENCE,
                        box( Point3 { 4, 1, 0 }, Vector3 { 1, 1, 1 }, lambertian( ColorRGB { 0.4, 0.8, 0.1 } ) ),
                        sphere( Point3 { 4, 1, 0 }, 1.2f, metallic( ColorRGB { 0.9, 0.2, 0.1 }, 0.1 ) ) ) );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >( 20, 4.0 / 3.0, 0.1f, 1000.0f );
                    camera->setLocal(
                        lookAt(
                            Point3 { 0, 2, 30 },
                            Point3 { 0, 1, 0 },
                            Vector3::Constants::UP ) );
                    camera->setFocusDistance( 10 );
                    camera->setAperture( 0.0f );
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
                scene->perform( SceneDebugDump( "scene.out" ) );

                return scene;
            }() );

        // Use soft RT by default
        // RenderSystem::getInstance()->useRTComputeRenderPath();
        RenderSystem::getInstance()->useRTSoftRenderPath();
    }
};

CRIMILD_CREATE_SIMULATION( Example, "RT: CSG" );
