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

#ifndef CRIMILD_CORE_RENDERING_FORMAT_
#define CRIMILD_CORE_RENDERING_FORMAT_

#include "Mathematics/Vector.hpp"

namespace crimild {

    /**
	   \brief Different format values

	   Suffixes:
	   - UNORM: Unsigned values normalized to [0.0, 1.0]
	   - SNORM: Signed values normalized to [-1.0, 1.0]
	   - UINT: Unsigned integers. No conversion perfomed
	   - SINT: Signed integers. No conversion performed
	   - UFLOAT: Unsigned float values. No conversion performed
	   - SFLOAT: Signed float values. No conversion performed
	   - SRGB: Same as UNORM. The value is interpreted to be in sRGB color space
	 */
    enum class Format {
        UNDEFINED,
        R8_UNORM,
        R8_SNORM,
        R8_UINT,
        R8_SINT,
        R8G8B8_UINT,
        R8G8B8A8_UINT,
        R8G8B8_UNORM,
        R8G8B8A8_UNORM,
        B8G8R8A8_UNORM,
        R16_UINT,
        R16_SFLOAT,
        R16G16_SFLOAT,
        R16G16B16_SFLOAT,
        R16G16B16A16_SFLOAT,
        R32_UINT,
        R32_SINT,
        R32_SFLOAT,
        R32G32_SFLOAT,
        R32G32B32_SFLOAT,
        R32G32B32A32_SFLOAT,
        R64_UINT,
        R64_SINT,
        R64_SFLOAT,
        DEPTH_16_UNORM,
        DEPTH_32_SFLOAT,
        DEPTH_16_UNORM_STENCIL_8_UINT,
        DEPTH_24_UNORM_STENCIL_8_UINT,
        DEPTH_32_SFLOAT_STENCIL_8_UINT,
        DEPTH_STENCIL_DEVICE_OPTIMAL, //< Whatever depth/stencil format is supported
        COLOR_SWAPCHAIN_OPTIMAL,      //< Whatever format the swapchain has
        INDEX_16_UINT = R16_UINT,
        INDEX_32_UINT = R32_UINT,
    };

    namespace utils {

        template< typename T >
        static Format getFormat( void ) noexcept { return Format::UNDEFINED; }

        template<>
        Format getFormat< UInt16 >( void ) noexcept { return Format::R16_UINT; }
        template<>
        Format getFormat< UInt32 >( void ) noexcept { return Format::R32_UINT; }
        template<>
        Format getFormat< Real32 >( void ) noexcept { return Format::R32_SFLOAT; }
        template<>
        Format getFormat< Vector2f >( void ) noexcept { return Format::R32G32_SFLOAT; }
        template<>
        Format getFormat< Vector3f >( void ) noexcept { return Format::R32G32B32_SFLOAT; }
        template<>
        Format getFormat< Vector4f >( void ) noexcept { return Format::R32G32B32A32_SFLOAT; }

        static UInt32 getFormatSize( Format format ) noexcept
        {
            switch ( format ) {
                case Format::R8G8B8A8_UINT:
                    return 4 * sizeof( UInt8 );
                case Format::R16_UINT:
                    return 1 * sizeof( UInt16 );
                case Format::R32_UINT:
                    return 1 * sizeof( UInt32 );
                case Format::R32_SFLOAT:
                    return 1 * sizeof( Real32 );
                case Format::R32G32_SFLOAT:
                    return 2 * sizeof( Real32 );
                case Format::R32G32B32_SFLOAT:
                    return 3 * sizeof( Real32 );
                case Format::R32G32B32A32_SFLOAT:
                    return 4 * sizeof( Real32 );
                default:
                    return 0;
            };
        }

    }

}

#endif
