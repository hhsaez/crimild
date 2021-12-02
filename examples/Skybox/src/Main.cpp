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
                getSettings()->set( "debug", true );

                auto scene = crimild::alloc< Group >();

                auto primitive = crimild::alloc< BoxPrimitive >(
                    BoxPrimitive::Params {
                        .type = Primitive::Type::TRIANGLES,
                        .layout = VertexP3N3TC2::getLayout(),
                    } );

                auto material = [] {
                    auto material = crimild::alloc< UnlitMaterial >();
                    material->setColor( ColorRGBA { 1.0f, 0.0f, 1.0f, 1.0f } );
                    material->setTexture(
                        [] {
                            auto texture = crimild::alloc< Texture >();
                            texture->imageView = [ & ] {
                                auto imageView = crimild::alloc< ImageView >();
                                imageView->image = Image::CHECKERBOARD_16;
                                return imageView;
                            }();
                            texture->sampler = [ & ] {
                                auto sampler = crimild::alloc< Sampler >();
                                sampler->setMinFilter( Sampler::Filter::NEAREST );
                                sampler->setMagFilter( Sampler::Filter::NEAREST );
                                sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
                                return sampler;
                            }();
                            return texture;
                        }() );
                    return material;
                }();

                math::fibonacciSquares( 20 ).each(
                    [ & ]( auto &it ) {
                        scene->attachNode(
                            [ & ] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive( primitive );
                                geometry->setLocal( translation( it.first + Vector3::Constants::UNIT_Z * it.second ) * scale( 0.25f * it.second ) );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial( material );
                                return geometry;
                            }() );
                    } );

                scene->attachNode(
                    [ & ] {
                        auto skybox = crimild::alloc< Skybox >(
                            [] {
                                auto texture = crimild::alloc< Texture >();
                                texture->imageView = [ & ] {
                                    auto imageView = crimild::alloc< ImageView >();
                                    imageView->image = ImageManager::getInstance()->loadCubemap(
                                        {
                                            .filePaths = {
                                                { .path = "assets/textures/right.tga" },
                                                { .path = "assets/textures/left.tga" },
                                                { .path = "assets/textures/top.tga" },
                                                { .path = "assets/textures/bottom.tga" },
                                                { .path = "assets/textures/back.tga" },
                                                { .path = "assets/textures/front.tga" },
                                            },
                                        } );
                                    return imageView;
                                }();
                                texture->sampler = [ & ] {
                                    auto sampler = crimild::alloc< Sampler >();
                                    sampler->setMinFilter( Sampler::Filter::LINEAR );
                                    sampler->setMagFilter( Sampler::Filter::LINEAR );
                                    sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
                                    sampler->setCompareOp( CompareOp::NEVER );
                                    return sampler;
                                }();
                                return texture;
                            }() );
                        return skybox;
                    }() );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal( translation( 0.0f, 10.0f, 100.0f ) );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Skybox" );
