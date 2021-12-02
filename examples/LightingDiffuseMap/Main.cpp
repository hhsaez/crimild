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

                scene->attachNode(
                    [] {
                        auto earth = crimild::alloc< Group >();
                        earth->attachNode(
                            [] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive(
                                    crimild::alloc< SpherePrimitive >(
                                        SpherePrimitive::Params {
                                            .type = Primitive::Type::TRIANGLES,
                                            .layout = VertexP3N3TC2::getLayout(),
                                        } ) );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                    [ & ] {
                                        auto material = crimild::alloc< LitMaterial >();
                                        material->setAlbedoMap(
                                            [] {
                                                auto texture = crimild::alloc< Texture >();
                                                texture->imageView = [ & ] {
                                                    auto imageView = crimild::alloc< ImageView >();
                                                    imageView->image = [ & ] {
                                                        return ImageManager::getInstance()->loadImage(
                                                            {
                                                                .filePath = {
                                                                    .path = "assets/textures/earth-diffuse.tga" },
                                                            } );
                                                    }();
                                                    return imageView;
                                                }();
                                                texture->sampler = crimild::alloc< Sampler >();
                                                return texture;
                                            }() );
                                        return material;
                                    }() );
                                return geometry;
                            }() );
                        return behaviors::withBehavior(
                        	earth,
                            behaviors::actions::rotate(
                            	Vector3::Constants::UP,
                                0.05f
                            )
                        );
                    }() );

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
                                        auto material = crimild::alloc< UnlitMaterial >();
                                        material->setColor( ColorRGBA::Constants::WHITE );
                                        return material;
                                    }() );
                                return geometry;
                            }() );
                        group->attachNode(
                            [] {
                                auto light = crimild::alloc< Light >( Light::Type::POINT );
                                light->setColor( ColorRGBA::Constants::WHITE );
                                light->setEnergy( 100.0f );
                                return light;
                            }() );
                        group->attachComponent< LambdaComponent >(
                            []( auto node, auto &clock ) {
                                auto speed = 0.1f;
                                auto t = speed * clock.getCurrentTime();
                                auto x = Numericf::remapCos( -5.0f, 5.0f, t );
                                auto y = 0.0f;
                                auto z = Numericf::remapSin( -5.0f, 5.0f, t );
                                node->setLocal( translation( x, y, z ) );
                            } );
                        return group;
                    }() );

                scene->attachNode(
                    [ & ] {
                        auto camera = crimild::alloc< Camera >();
                        camera->setLocal( translation( 0.0f, 0.0f, 3.0f ) );
                        Camera::setMainCamera( camera );
                        return camera;
                    }() );

                scene->attachNode( crimild::alloc< Skybox >( ColorRGB { 0.0045f, 0.005f, 0.0125f } ) );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Lighting: Diffuse Map" );
