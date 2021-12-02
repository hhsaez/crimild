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

                auto textureBuilder = []( crimild::Bool mipmapping ) {
                    auto texture = crimild::alloc< Texture >();
                    texture->imageView = [ & ] {
                        auto imageView = crimild::alloc< ImageView >();
                        imageView->image = Image::CHECKERBOARD_128;
                        // Enable/Disable mipmapping at the image view
                        imageView->mipLevels = mipmapping ? 0 : 1;
                        return imageView;
                    }();
                    texture->sampler = [ & ] {
                        auto sampler = crimild::alloc< Sampler >();
                        sampler->setMinFilter( Sampler::Filter::NEAREST );
                        sampler->setMagFilter( Sampler::Filter::NEAREST );
                        sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
                        if ( mipmapping ) {
                            sampler->setMaxLod( texture->imageView->image->getMipLevels() );
                        }
                        return sampler;
                    }();
                    return texture;
                };

                auto quadBuilder = [ & ]( const Vector3f &position, float scale, SharedPointer< Texture > const &texture ) {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive(
                        crimild::alloc< QuadPrimitive >(
                            QuadPrimitive::Params {
                                .layout = VertexP3N3TC2::getLayout(),
                                .size = 0.5f * Vector2::Constants::ONE,
                            } ) );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [ & ] {
                            auto material = crimild::alloc< UnlitMaterial >();
                            material->setTexture( texture );
                            return material;
                        }() );
                    geometry->setLocal( translation( position ) * crimild::scale( scale ) );
                    return geometry;
                };

                auto sampleBuilder = [ & ]( const Vector3f &position, crimild::Bool mipmapping ) {
                    auto texture = textureBuilder( mipmapping );
                    auto group = crimild::alloc< Group >();

                    math::fibonacciSquares( 8 ).each(
                        [ & ]( auto &it ) {
                            group->attachNode(
                                quadBuilder(
                                    it.first,
                                    0.9f * it.second,
                                    texture ) );
                        } );

                    group->setLocal( translation( position ) );
                    return group;
                };

                scene->attachNode( sampleBuilder( Vector3 { -7.0f, 7.0f, 0.0f }, true ) );
                scene->attachNode( sampleBuilder( Vector3 { -7.0f, -19.0f, 0.0f }, false ) );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal( translation( 0.0f, 0.0f, 80.0f ) );
                    return camera;
                }() );
                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Texture Mipmaps" );
