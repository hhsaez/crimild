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

#ifndef CRIMILD_MATHEMATICS_PERSPECTIVE_
#define CRIMILD_MATHEMATICS_PERSPECTIVE_

#include "Matrix4.hpp"
#include "trigonometry.hpp"

namespace crimild {

    [[nodiscard]] constexpr Matrix4 perspective( real_t l, real_t r, real_t b, real_t t, real_t n, real_t f ) noexcept
    {
#if CRIMILD_USE_DEPTH_RANGE_ZERO_TO_ONE
        return Matrix4 {
            Vector4 { 2 * n / ( r - l ), 0, 0, 0 },
            Vector4 { 0, 2 * n / ( t - b ), 0, 0 },
            Vector4 { ( r + l ) / ( r - l ), ( t + b ) / ( t - b ), f / ( n - f ), -1 },
            Vector4 { 0, 0, -f * n / ( f - n ), 0 }
        };
#else
        return Matrix4 {
            Vector4 { 2 * n / ( r - l ), 0, 0, 0 },
            Vector4 { 0, 2 * n / ( t - b ), 0, 0 },
            Vector4 { ( r + l ) / ( r - l ), ( t + b ) / ( t - b ), -( f + n ) / ( f - n ), -1 },
            Vector4 { 0, 0, -2 * f * n / ( f - n ), 0 }
        };
#endif
    }

    /**
       \brief Computes a perspective projecting using vertical FoV and aspect

       \param fov Vertical field of view, in degrees
       \param aspect Aspect ratio
       \param near Near clipping plane
       \param far Far clipping plane

       \todo Make this function constexpr
     */
    [[nodiscard]] static Matrix4 perspective( degrees_t fov, real_t aspect, real_t near, real_t far ) noexcept
    {
        const auto c = real_t( 1 ) / tan( real_t( 0.5 ) * radians( fov ) );
        const auto a = aspect;
        const auto n = near;
        const auto f = far;

#if CRIMILD_USE_DEPTH_RANGE_ZERO_TO_ONE
        return Matrix4 {
            Vector4 { c / a, 0, 0, 0 },
            Vector4 { 0, c, 0, 0 },
            Vector4 { 0, 0, f / ( n - f ), -1 },
            Vector4 { 0, 0, -f * n / ( f - n ), 0 }
        };
#else
        return Matrix4 {
            Vector4 { c / a, 0, 0, 0 },
            Vector4 { 0, c, 0, 0 },
            Vector4 { 0, 0, -( f + n ) / ( f - n ), -1 },
            Vector4 { 0, 0, real_t( -2 ) * f * n / ( f - n ), 0 }
        };
#endif
    }

}

#endif
