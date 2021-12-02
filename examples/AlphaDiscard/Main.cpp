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
                        geometry->attachPrimitive( crimild::alloc< QuadPrimitive >() );
                        geometry->setLocal(
                            [] {
                                Transformation t;
                                t.rotate().fromAxisAngle( -Vector3f::UNIT_X, Numericf::HALF_PI );
                                t.setScale( 10.0f );
                                return t;
                            }() );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setColor( RGBAColorf( 0.166, 0.5f, 0.032f, 1.0f ) );
                                return material;
                            }() );
                        return geometry;
                    }() );

                auto material = [] {
                    auto material = crimild::alloc< UnlitMaterial >();
                    material->setTexture(
                        [] {
                            auto texture = crimild::alloc< Texture >();
                            texture->imageView = [] {
                                auto imageView = crimild::alloc< ImageView >();
                                imageView->image = ImageManager::getInstance()->loadImage(
                                    {
                                        .filePath = {
                                            .path = "assets/textures/grass.png" },
                                    } );
                                return imageView;
                            }();
                            texture->sampler = [] {
                                auto sampler = crimild::alloc< Sampler >();
                                sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_EDGE );
                                return sampler;
                            }();
                            return texture;
                        }() );
                    return material;
                }();

                auto grassPatch = crimild::alloc< QuadPrimitive >(
                    QuadPrimitive::Params {} );

                auto rnd = Random::Generator( 1982 );

                Array< SharedPointer< Geometry > >( 10 ).fill(
                                                            [ & ]( auto index ) {
                                                                auto geometry = crimild::alloc< Geometry >();
                                                                geometry->attachPrimitive( grassPatch );
                                                                geometry->local().setTranslate(
                                                                    rnd.generate( -5.0f, 5.0f ),
                                                                    1.0f,
                                                                    rnd.generate( -5.0f, 5.0f ) );
                                                                geometry->attachComponent< MaterialComponent >()->attachMaterial( material );
                                                                return geometry;
                                                            } )
                    .sort(
                        []( const auto &lhs, const auto &rhs ) {
                            return lhs->getLocal().getTranslate().z() < rhs->getLocal().getTranslate().z();
                        } )
                    .each(
                        [ & ]( auto &geometry ) {
                            scene->attachNode( geometry );
                        } );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->local().setTranslate( 3.0f, 5.0f, 10.0f );
                    camera->local().lookAt( Vector3f::ZERO );
                    return camera;
                }() );

                return scene;
            }() );

        setComposition(
            [ scene = getScene() ] {
                using namespace crimild::compositions;
                return present( renderScene( scene ) );
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Alpha Discard" );
