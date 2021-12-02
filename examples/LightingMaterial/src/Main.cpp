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

                auto sphere = []( auto position, auto props ) {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive(
                        crimild::alloc< SpherePrimitive >(
                            SpherePrimitive::Params {
                                .type = Primitive::Type::TRIANGLES,
                                .layout = VertexP3N3TC2::getLayout(),
                            } ) );
                    geometry->local().setTranslate( position );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ & ] {
                            auto material = crimild::alloc< SimpleLitMaterial >( props );
                            return material;
                        }() );
                    return geometry;
                };

                // From: http://devernay.free.fr/cours/opengl/materials.html
                auto props = Array< SimpleLitMaterial::Props > {
                    /* emerald */ { RGBAColorf( 0.0215f, 0.1745f, 0.0215f, 1.0f ), RGBAColorf( 0.07568f, 0.61424f, 0.07568f, 1.0f ), RGBAColorf( 0.633f, 0.727811f, 0.633f, 1.0f ), 128.0f * 0.6f },
                    /* jade */ { RGBAColorf( 0.135f, 0.2225f, 0.1575f, 1.0f ), RGBAColorf( 0.54f, 0.89f, 0.63f, 1.0f ), RGBAColorf( 0.316228f, 0.316228f, 0.316228f, 1.0f ), 128.0f * 0.1f },
                    /* obsidian */ { RGBAColorf( 0.05375f, 0.05f, 0.06625f, 1.0f ), RGBAColorf( 0.18275f, 0.17f, 0.22525f, 1.0f ), RGBAColorf( 0.332741f, 0.328634f, 0.346435f, 1.0f ), 128.0f * 0.3f },
                    /* pearl */ { RGBAColorf( 0.25f, 0.20725f, 0.20725f, 1.0f ), RGBAColorf( 1.0f, 0.829f, 0.829f, 1.0f ), RGBAColorf( 0.296648f, 0.296648f, 0.296648f, 1.0f ), 128.0f * 0.088f },
                    /* ruby */ { RGBAColorf( 0.1745f, 0.01175f, 0.01175f, 1.0f ), RGBAColorf( 0.61424f, 0.04136f, 0.04136f, 1.0f ), RGBAColorf( 0.727811f, 0.626959f, 0.626959f, 1.0f ), 128.0f * 0.6f },
                    /* turquoise */ { RGBAColorf( 0.1f, 0.18725f, 0.1745f, 1.0f ), RGBAColorf( 0.396f, 0.74151f, 0.69102f, 1.0f ), RGBAColorf( 0.297254f, 0.30829f, 0.306678f, 1.0f ), 128.0f * 0.1f },
                    /* brass */ { RGBAColorf( 0.329412f, 0.223529f, 0.027451f, 1.0f ), RGBAColorf( 0.780392f, 0.568627f, 0.113725f, 1.0f ), RGBAColorf( 0.992157f, 0.941176f, 0.807843f ), 128.0f * 0.21794872f },
                    /* bronze */ { RGBAColorf( 0.2125f, 0.1275f, 0.054f, 1.0f ), RGBAColorf( 0.714f, 0.4284f, 0.18144f, 1.0f ), RGBAColorf( 0.393548f, 0.271906f, 0.166721f, 1.0f ), 128.0f * 0.2f },
                    /* chrome */ { RGBAColorf( 0.25f, 0.25f, 0.25f, 1.0f ), RGBAColorf( 0.4f, 0.4f, 0.4f, 1.0f ), RGBAColorf( 0.774597f, 0.774597f, 0.774597f, 1.0f ), 128.0f * 0.6f },
                    /* copper */ { RGBAColorf( 0.19125f, 0.0735f, 0.0225f, 1.0f ), RGBAColorf( 0.7038f, 0.27048f, 0.0828f, 1.0f ), RGBAColorf( 0.256777f, 0.137622f, 0.086014f, 1.0f ), 128.0f * 0.1f },
                    /* gold */ { RGBAColorf( 0.24725f, 0.1995f, 0.0745f, 1.0f ), RGBAColorf( 0.75164f, 0.60648f, 0.22648f, 1.0f ), RGBAColorf( 0.628281f, 0.555802f, 0.366065f, 1.0f ), 128.0f * 0.4f },
                    /* silver */ { RGBAColorf( 0.19225f, 0.19225f, 0.19225f, 1.0f ), RGBAColorf( 0.50754f, 0.50754f, 0.50754f, 1.0f ), RGBAColorf( 0.508273f, 0.508273f, 0.508273f, 1.0f ), 128.0f * 0.4f },
                    /* black plastic */ { RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ), RGBAColorf( 0.01f, 0.01f, 0.01f, 1.0f ), RGBAColorf( 0.50f, 0.50f, 0.50f, 1.0f ), 128.0f * 0.25f },
                    /* cyan plastic */ { RGBAColorf( 0.0f, 0.1f, 0.06f, 1.0f ), RGBAColorf( 0.0f, 0.50980392f, 0.50980392f, 1.0f ), RGBAColorf( 0.50196078f, 0.50196078f, 0.50196078f, 1.0f ), 128.0f * 0.25f },
                    /* green plastic */ { RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ), RGBAColorf( 0.1f, 0.35f, 0.1f, 1.0f ), RGBAColorf( 0.45f, 0.55f, 0.45f, 1.0f ), 128.0f * 0.25f },
                    /* red plastic */ { RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ), RGBAColorf( 0.5f, 0.0f, 0.0f, 1.0f ), RGBAColorf( 0.7f, 0.6f, 0.6f, 1.0f ), 128.0f * 0.25f },
                    /* white plastic */ { RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ), RGBAColorf( 0.55f, 0.55f, 0.55f, 1.0f ), RGBAColorf( 0.70f, 0.70f, 0.70f, 1.0f ), 128.0f * 0.25f },
                    /* yellow plastic */ { RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ), RGBAColorf( 0.5f, 0.5f, 0.0f, 1.0f ), RGBAColorf( 0.60f, 0.60f, 0.50f, 1.0f ), 128.0f * 0.25f },
                    /* black rubber */ { RGBAColorf( 0.02f, 0.02f, 0.02f, 1.0f ), RGBAColorf( 0.01f, 0.01f, 0.01f, 1.0f ), RGBAColorf( 0.4f, 0.4f, 0.4f, 1.0f ), 128.0f * 0.078125f },
                    /* cyan rubber */ { RGBAColorf( 0.0f, 0.05f, 0.05f, 1.0f ), RGBAColorf( 0.4f, 0.5f, 0.5f, 1.0f ), RGBAColorf( 0.04f, 0.7f, 0.7f, 1.0f ), 128.0f * 0.078125f },
                    /* green rubber */ { RGBAColorf( 0.0f, 0.05f, 0.0f, 1.0f ), RGBAColorf( 0.4f, 0.5f, 0.4f, 1.0f ), RGBAColorf( 0.04f, 0.7f, 0.04f, 1.0f ), 128.0f * 0.078125f },
                    /* red rubber */ { RGBAColorf( 0.05f, 0.0f, 0.0f, 1.0f ), RGBAColorf( 0.5f, 0.4f, 0.4f, 1.0f ), RGBAColorf( 0.7f, 0.04f, 0.04f, 1.0f ), 128.0f * 0.078125f },
                    /* white rubber	*/ { RGBAColorf( 0.05f, 0.05f, 0.05f, 1.0f ), RGBAColorf( 0.5f, 0.5f, 0.5f, 1.0f ), RGBAColorf( 0.7f, 0.7f, 0.7f, 1.0f ), 128.0f * 0.078125f },
                    /* yellow rubber */ { RGBAColorf( 0.05f, 0.05f, 0.0f, 1.0f ), RGBAColorf( 0.5f, 0.5f, 0.4f, 1.0f ), RGBAColorf( 0.7f, 0.7f, 0.04f, 1.0f ), 128.0f, 0.078125 },
                };

                props.each(
                    [ &, i = 0 ]( auto &p ) mutable {
                        auto pos = Vector3f( 2.5f * ( i % 6 ), -2.5f * ( i / 6 ), 0.0f );
                        scene->attachNode( sphere( pos, p ) );
                        i++;
                    } );

                scene->attachNode(
                    [] {
                        auto group = crimild::alloc< Group >();
                        group->attachNode(
                            [] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive(
                                    crimild::alloc< SpherePrimitive >(
                                        SpherePrimitive::Params {
                                            .type = Primitive::Type::TRIANGLES,
                                            .layout = VertexP3N3TC2::getLayout(),
                                            .radius = 0.1f,
                                        } ) );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                    [] {
                                        // TODO: should use unlit material instead
                                        auto material = crimild::alloc< UnlitMaterial >();
                                        material->setColor( RGBAColorf::ONE );
                                        return material;
                                    }() );
                                return geometry;
                            }() );
                        group->attachNode(
                            [] {
                                auto light = crimild::alloc< Light >();
                                return light;
                            }() );
                        group->attachComponent< LambdaComponent >(
                            []( auto node, auto &clock ) {
                                auto speed = 0.75f;
                                auto t = speed * clock.getCurrentTime();
                                auto x = Numericf::remap( -1.0f, 1.0f, -3.0f, 15.0f, Numericf::cos( t ) * Numericf::sin( t ) );
                                auto y = Numericf::remapSin( -0.5f, -7.0f, t );
                                auto z = 5.0f;
                                node->local().setTranslate( x, y, z );
                            } );
                        return group;
                    }() );

                scene->attachNode(
                    [ & ] {
                        auto camera = crimild::alloc< Camera >();
                        camera->local().setTranslate( 6.5f, -4.0f, 15.0f );
                        Camera::setMainCamera( camera );
                        return camera;
                    }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Lighting: Materials" );
