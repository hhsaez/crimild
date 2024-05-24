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

#include "Rendering/ImageManager.hpp"

#include "Crimild_Foundation.hpp"
#include "Rendering/ImageTGA.hpp"

using namespace crimild;

SharedPointer< Image > ImageManager::loadImage( ImageDescriptor const &descriptor ) const noexcept
{
    if ( descriptor.filePath.getExtension() != "tga" ) {
        CRIMILD_LOG_WARNING( "Invalid image file ", descriptor.filePath.path );
        return nullptr;
    }

    return crimild::alloc< ImageTGA >( descriptor.filePath.getAbsolutePath() );
}

SharedPointer< Image > ImageManager::loadCubemap( CubemapDescriptor const &descriptor ) const noexcept
{
    // This is not ideal. We create several images just to delete them later and
    // create a new one with the whole data... Maybe derived classes can handle
    // this one in a better way

    auto images = descriptor.filePaths.map(
        [ & ]( auto &path ) {
            return loadImage(
                ImageDescriptor {
                    .filePath = path,
                }
            );
        }
    );

    if ( images.empty() ) {
        return nullptr;
    }

    auto layerCount = images.size();
    auto firstImage = images[ 0 ];
    auto format = firstImage->format;
    auto w = firstImage->extent.width;
    auto h = firstImage->extent.height;

    auto image = crimild::alloc< Image >();
    image->type = Image::Type::IMAGE_2D_CUBEMAP;
    image->format = format;
    image->setLayerCount( layerCount );
    image->extent = {
        .width = w,
        .height = h,
        .depth = 1,
    };
    image->setBufferView(
        crimild::alloc< BufferView >(
            BufferView::Target::IMAGE,
            crimild::alloc< Buffer >(
                [ & ] {
                    auto data = ByteArray( layerCount * firstImage->getBufferView()->getLength() );
                    Size offset = 0;
                    images.each(
                        [ &, i = 0 ]( auto &img ) mutable {
                            memcpy( &data[ offset ], img->getBufferView()->getData(), img->getBufferView()->getLength() );
                            offset += img->getBufferView()->getLength();
                        }
                    );
                    return data;
                }()
            )
        )
    );

    return image;
}
