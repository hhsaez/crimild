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
        auto loadTexture = []( std::string path ) {
            auto texture = crimild::alloc< Texture >();
            texture->imageView = [ & ] {
                auto imageView = crimild::alloc< ImageView >();
                imageView->image = [ & ] {
                    return ImageManager::getInstance()->loadImage(
                        { .filePath = {
                              .path = path,
                          } } );
                }();
                return imageView;
            }();
            texture->sampler = [ & ] {
                auto sampler = crimild::alloc< Sampler >();
                sampler->setMinFilter( Sampler::Filter::LINEAR );
                sampler->setMagFilter( Sampler::Filter::LINEAR );
                return sampler;
            }();
            return texture;
        };

        auto albedo = loadTexture( "assets/textures/rustediron2_basecolor.png" );
        auto metallic = loadTexture( "assets/textures/rustediron2_metallic.png" );
        auto roughness = loadTexture( "assets/textures/rustediron2_roughness.png" );
        auto normal = loadTexture( "assets/textures/rustediron2_normal.png" );

        setScene( [ & ] {
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
                    geometry->local().setTranslate( 2.5f * Vector3f( -3.0f + x, 3.0f - y, 0 ) );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ x, y, albedo, metallic, roughness, normal ] {
                            auto material = crimild::alloc< LitMaterial >();
                            material->setAlbedoMap( albedo );
                            material->setMetallic( 1.0f - float( y ) / 6.0f );
                            material->setMetallicMap( metallic );
                            material->setRoughness( float( x ) / 6.0f );
                            material->setRoughnessMap( roughness );
                            material->setNormalMap( normal );

                            return material;
                        }() );
                    scene->attachNode( geometry );
                }
            }

            auto createLight = []( const auto &color, auto energy ) {
                auto group = crimild::alloc< Group >();
                auto light = [ & ] {
                    auto light = crimild::alloc< Light >( Light::Type::POINT );
                    light->setColor( color );
                    light->setEnergy( energy );
                    return light;
                }();
                group->attachNode( light );

                auto lightPositionIndicator = [ & ] {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive(
                        crimild::alloc< SpherePrimitive >(
                            SpherePrimitive::Params {
                                .type = Primitive::Type::TRIANGLES,
                                .layout = VertexP3N3TC2::getLayout(),
                                .radius = 0.1f,
                            } ) );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ & ] {
                            auto material = crimild::alloc< UnlitMaterial >();
                            material->setColor( color );
                            material->setCastShadows( false );
                            return material;
                        }() );
                    return geometry;
                }();
                group->attachNode( lightPositionIndicator );

                group->attachComponent< LambdaComponent >(
                    [ maxX = Random::generate( 5.0, 25.0 ),
                      maxY = Random::generate( 5.0, 15.0 ),
                      maxZ = Random::generate( 5.0, 15.0 ),
                      direction = Numericf::sign( Random::generate( -1.0f, 1.0f ) ),
                      speed = Random::generate( 0.125f, 0.5f ) ]( auto node, auto &clock ) {
                        auto t = Numericf::sign( direction ) * speed * clock.getCurrentTime();
                        auto x = Numericf::remap( -1.0f, 1.0f, -maxX, maxX, Numericf::cos( t ) * Numericf::sin( t ) );
                        auto y = Numericf::remapSin( -maxY, maxY, t );
                        auto z = Numericf::remapCos( -maxZ, maxZ, t );
                        if ( !Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_SPACE ) ) {
                            node->local().setTranslate( x, y, z );
                        }
                    } );
                return group;
            };

            scene->attachNode( createLight( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ), 500.0f ) );

            scene->attachNode( crimild::alloc< Skybox >( RGBColorf( 0.25f, 0.25f, 0.5f ) ) );

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

CRIMILD_CREATE_SIMULATION( Example, "PBR: Texture" );
