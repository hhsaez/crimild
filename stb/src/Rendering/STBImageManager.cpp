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

#include "Rendering/STBImageManager.hpp"
#include "Rendering/Image.hpp"
#include "Foundation/Log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace crimild;

SharedPointer< Image > crimild::stb::ImageManager::loadImage( ImageDescriptor const &descriptor ) const noexcept
{
    int width, height, channels;

    // Fix image orientation if needed
    stbi_set_flip_vertically_on_load( 0 );

    stbi_uc *pixels = stbi_load(
    	descriptor.filePath.getAbsolutePath().c_str(),
    	&width,
        &height,
        &channels,
        STBI_rgb_alpha // Force loaded image to be RGBA
    );

    if ( pixels == nullptr ) {
        CRIMILD_LOG_WARNING( "Failed to load image", descriptor.filePath.getAbsolutePath() );
        return nullptr;
    }

	auto image = crimild::alloc< Image >();
	image->format = Format::R8G8B8A8_UNORM;
	image->extent = {
		.width = crimild::Real32( width ),
		.height = crimild::Real32( height ),
		.depth = 1.0f,
	};

	auto size = width * height * 4 * sizeof( stbi_uc );
	image->data.resize( size );
	memcpy( image->data.getData(), pixels, size );

    stbi_image_free( pixels );

    return image;
}
