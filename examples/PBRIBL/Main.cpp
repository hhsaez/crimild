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

                auto primitive = crimild::alloc< SpherePrimitive >(
                    SpherePrimitive::Params {
                        .type = Primitive::Type::TRIANGLES,
                        .layout = VertexP3N3TC2::getLayout(),
                    } );

                for ( auto y = 0; y < 7; ++y ) {
                    for ( auto x = 0; x < 7; ++x ) {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive( primitive );
                        geometry->setLocal( translation( 2.5f * Vector3f { -3.0f + x, 3.0f - y, 0 } ) );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [ x, y ] {
                                auto material = crimild::alloc< materials::PrincipledBSDF >();
                                material->setMetallic( 1.0f - float( y ) / 6.0f );
                                material->setRoughness( Real( x ) / 6.0f );
                                return material;
                            }() );
                        scene->attachNode( geometry );
                    }
                }

                scene->attachNode(
                    crimild::alloc< Skybox >(
                        [ settings = getSettings() ] {
                            auto texture = crimild::alloc< Texture >();
                            texture->imageView = [ settings ] {
                                auto imageView = crimild::alloc< ImageView >();
                                imageView->image = ImageManager::getInstance()->loadImage(
                                    {
                                        .filePath = {
                                            .pathType = settings->hasKey( "skybox" ) ? FilePath::PathType::ABSOLUTE : FilePath::PathType::RELATIVE,
                                            .path = settings->get< std::string >( "skybox", "assets/textures/Newport_Loft_Ref.hdr" ),
                                        },
                                        .hdr = true,
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
                        }() ) );

                auto createLight = []( const auto &position ) {
                    auto light = crimild::alloc< Light >( Light::Type::POINT );
                    light->setLocal( translation( position ) );
                    light->setColor( ColorRGBA { 55, 55, 55 } );
                    return light;
                };

                scene->attachNode( createLight( Vector3 { -15.0f, +15.0f, 10.0f } ) );
                scene->attachNode( createLight( Vector3 { +15.0f, +15.0f, 10.0f } ) );
                scene->attachNode( createLight( Vector3 { -15.0f, -15.0f, 10.0f } ) );
                scene->attachNode( createLight( Vector3 { +15.0f, -15.0f, 10.0f } ) );

                scene->attachComponent< LambdaComponent >( []( auto, auto ) {
                    if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_F8 ) ) {
                        // std::cout << "take screenshot" << std::endl;
                    }
                } );

                scene->attachNode(
                    [ & ] {
                        auto camera = crimild::alloc< Camera >();
                        camera->setLocal( lookAt( Point3 { 15, 5, 20 }, Point3 { 0, 0, 0 }, Vector3 { 0, 1, 0 } ) );
                        camera->attachComponent< FreeLookCameraComponent >();
                        return camera;
                    }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "PBR: IBL" );
