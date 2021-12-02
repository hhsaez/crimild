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
        auto rnd = Random::Generator( 1982 );

        setScene( [ & ] {
            auto scene = crimild::alloc< Group >();

            scene->attachNode(
                [] {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive(
                        crimild::alloc< BoxPrimitive >(
                            BoxPrimitive::Params {
                                .type = Primitive::Type::TRIANGLES,
                                .layout = VertexP3N3TC2::getLayout(),
                                .size = Vector3f( 5.0, 5.0, 50.0f ),
                                .invertFaces = true,
                            } ) );

                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ & ] {
                            auto material = crimild::alloc< LitMaterial >();
                            return material;
                        }() );
                    return geometry;
                }() );

            scene->attachNode(
                [] {
                    auto createLight = []( const auto &position, const auto &color ) {
                        auto light = crimild::alloc< Light >( Light::Type::POINT );
                        light->setAttenuation( Vector3f( 1.0f, 0.0f, 0.5f ) );
                        light->setColor( color );
                        light->local().setTranslate( position );
                        return light;
                    };

                    auto group = crimild::alloc< Group >();
                    group->attachNode( createLight( Vector3f( 0.0f, 0.0f, -6.0f ), RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
                    group->attachNode( createLight( Vector3f( -1.4f, -1.9f, -4.0f ), RGBAColorf( 0.0f, 1.0f, 0.0f, 1.0f ) ) );
                    group->attachNode( createLight( Vector3f( 0.0f, -1.8f, -9.0f ), RGBAColorf( 0.0f, 0.0f, 1.0f, 1.0f ) ) );
                    group->attachNode( createLight( Vector3f( 0.8f, -1.7f, -49.5f ), RGBAColorf( 200.0f, 200.0f, 200.0f, 1.0f ) ) );
                    return group;
                }() );

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
    }
};

CRIMILD_CREATE_SIMULATION( Example, "HDR" );
