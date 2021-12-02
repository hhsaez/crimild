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

void updateImageData( SharedPointer< Image > &image )
{
    const auto width = image->extent.width;
    const auto height = image->extent.height;
    const auto bpp = 4;

    for ( int y = 0; y < width; y++ ) {
        for ( int x = 0; x < width; x++ ) {
            for ( int i = 0; i < bpp; i++ ) {
                image->data[ y * width * bpp + x * bpp + i ] = Random::generate< UInt8 >( 0, 255 );
            }
        }
    }
}

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
                        geometry->attachPrimitive(
                            crimild::alloc< QuadPrimitive >(
                                QuadPrimitive::Params {
                                    .layout = VertexP3N3TC2::getLayout(),
                                } ) );

                        auto image = [] {
                            auto image = crimild::alloc< Image >();
                            crimild::Int32 width = 128;
                            crimild::Int32 height = 128;
                            crimild::Int32 bpp = 4;
                            image->extent = {
                                .width = crimild::Real32( width ),
                                .height = crimild::Real32( height ),
                                .depth = 1,
                            };
                            image->format = Format::R8G8B8A8_UNORM;
                            image->data = ByteArray( width * height * bpp );

                            updateImageData( image );
                            image->setBufferView(
                                [ & ] {
                                    auto buffer = crimild::alloc< Buffer >( ByteArray( width * height * bpp ) );
                                    auto bufferView = crimild::alloc< BufferView >( BufferView::Target::IMAGE, buffer );
                                    bufferView->setUsage( BufferView::Usage::DYNAMIC );
                                    return bufferView;
                                }() );
                            return image;
                        }();

                        geometry->attachComponent< LambdaComponent >(
                            [ image ]( auto node, auto &clock ) mutable {
                                updateImageData( image );
                            } );

                        geometry->attachComponent< MaterialComponent >(
                            [ image ] {
                                auto material = crimild::alloc< UnlitMaterial >();
                                material->setTexture(
                                    [ image ] {
                                        auto texture = crimild::alloc< Texture >();
                                        texture->imageView = [ image ] {
                                            auto imageView = crimild::alloc< ImageView >();
                                            imageView->image = image;
                                            return imageView;
                                        }();
                                        texture->sampler = [ & ] {
                                            auto sampler = crimild::alloc< Sampler >();
                                            sampler->setMinFilter( Sampler::Filter::NEAREST );
                                            sampler->setMagFilter( Sampler::Filter::NEAREST );
                                            return sampler;
                                        }();
                                        return texture;
                                    }() );
                                return material;
                            }() );
                        return geometry;
                    }() );
                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal( translation( 0.0f, 0.0f, 3.0f ) );
                    return camera;
                }() );
                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Textures" );
