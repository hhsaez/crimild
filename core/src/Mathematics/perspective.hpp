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

#include "Mathematics/Matrix4.hpp"

namespace crimild {

    [[nodiscard]] constexpr Matrix4 perspective( Real l, Real r, Real b, Real t, Real n, Real f ) noexcept
    {
        // clang-format off
        return Matrix4 {
            Real( 2 ) * n / ( r - l ), 0, ( r + l ) / ( r - l ), 0,
            0, Real( 2 ) * n / ( t - b ), ( t + b ) / ( t - b ), 0,
            0, 0, n / ( f - n ), f * n / ( f - n ),
            0, 0, -1, 0,
        };
        // clang-format on
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Matrix4 perspective( Real fov, Real a, Real n, Real f ) noexcept
    {
        // fov: vertical field of view

        const auto c = Real( 1 ) / tan( Real( 0.5 ) * fov );

        // clang-format off
        return Matrix4 {
            c / a, 0, 0, 0,
            0, -c, 0, 0,
            0, 0, n / ( f - n ), f * n / ( f - n ),
            0, 0, -1, 0,
        };
        // clang-format on
    }

}

#endif
