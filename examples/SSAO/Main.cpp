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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
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

            auto material = crimild::alloc< materials::PrincipledBSDF >();

            auto spheres = crimild::alloc< Group >();
            for ( int line = 0; line < 5; line++ ) {
                for ( float x = 0; x < line; x++ ) {
                    for ( float z = 0; z < line; z++ ) {
                        spheres->attachNode( [ & ] {
                            auto geometry = crimild::alloc< Geometry >();
                            geometry->attachPrimitive( crimild::alloc< SpherePrimitive >( SpherePrimitive::Params {} ) );
                            geometry->setLocal(
                                translation(
                                    2 * ( x - 0.5f * line ),
                                    7.0f - 1.5f * line,
                                    2 * ( z - 0.5f * line ) ) );
                            geometry->attachComponent< MaterialComponent >( material );
                            return geometry;
                        }() );
                    }
                }
            }
            scene->attachNode( spheres );

            scene->attachNode( [ & ] {
                auto geometry = crimild::alloc< Geometry >();
                auto scale = 10.0f;
                geometry->attachPrimitive(
                    crimild::alloc< BoxPrimitive >( BoxPrimitive::Params {
                        .size = scale * Vector3::Constants::ONE, .invertFaces = true } ) );
                geometry->attachComponent< MaterialComponent >( material );
                geometry->setLocal( translation( 0.0f, scale, 0.0f ) );
                return geometry;
            }() );

            scene->attachNode( crimild::alloc< Skybox >( ColorRGB { 0.75f, 0.75f, 0.75f } ) );

            scene->attachNode( [] {
                auto camera = crimild::alloc< Camera >();
                camera->setLocal(
                    lookAt(
                        Point3 { 0, 10, 10 },
                        Point3 { -1.0f, 2.0f, 0.0f },
                        Vector3 { 0, 1, 0 } ) );
                camera->attachComponent< FreeLookCameraComponent >();
                Camera::setMainCamera( camera );
                return camera;
            }() );

            scene->perform( StartComponents() );

            return scene;
        }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "SSAO" );