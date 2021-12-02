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
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive( crimild::alloc< BoxPrimitive >( BoxPrimitive::Params { .invertFaces = true } ) );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [] {
                                auto loadTexture = []( auto fileName ) {
                                    auto texture = crimild::alloc< Texture >();
                                    texture->imageView = [ & ] {
                                        auto imageView = crimild::alloc< ImageView >();
                                        imageView->image = [ & ] {
                                            return ImageManager::getInstance()->loadImage(
                                                {
                                                    .filePath = {
                                                        .path = fileName,
                                                    },
                                                } );
                                        }();
                                        return imageView;
                                    }();
                                    texture->sampler = [ & ] {
                                        auto sampler = crimild::alloc< Sampler >();
                                        return sampler;
                                    }();
                                    return texture;
                                };

                                auto material = crimild::alloc< LitMaterial >();
                                material->setMetallic( 0.0f );
                                material->setAlbedoMap( loadTexture( "assets/textures/stone-color.tga" ) );
                                material->setRoughnessMap( loadTexture( "assets/textures/stone-specular.tga" ) );
                                material->setNormalMap( loadTexture( "assets/textures/stone-normal.tga" ) );
                                return material;
                            }() );
                        return behaviors::withBehavior(
                        	geometry,
                         	behaviors::actions::rotate(
                                normalize( Vector3::Constants::ONE ),
                                0.01f ) );
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
                                            .radius = 0.05f,
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
                                light->setEnergy( 1.0f );
                                return light;
                            }() );
                        group->attachComponent< LambdaComponent >(
                            [ t = 0.0f ]( auto node, auto &clock ) mutable {
                                auto speed = 0.005f;
                                auto x = Numericf::remap( -1.0f, 1.0f, -0.95f, 0.95f, Numericf::cos( t ) * Numericf::sin( t ) );
                                auto y = Numericf::remapSin( -0.95f, 0.95f, t );
                                auto z = Numericf::remapCos( -0.95f, 0.95f, t );
                                if ( !Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_SPACE ) ) {
                                    node->setLocal( translation( x, y, z ) );
                                    t += speed + clock.getDeltaTime();
                                }
                            } );
                        return group;
                    }() );

                scene->attachNode(
                    [ & ] {
                        auto camera = crimild::alloc< Camera >();
                        camera->setLocal( translation( 0.0f, 0.0f, 3.0f ) );
                        return camera;
                    }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Normal Mapping" );
