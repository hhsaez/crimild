/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_SCALE_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_SCALE_

#include "Transformation.hpp"

namespace crimild {

    [[nodiscard]] static constexpr Transformation scale( const Vector3 &scale ) noexcept
    {
        const auto m = Matrix4(
            Vector4 { scale.x, 0, 0, 0 },
            Vector4 { 0, scale.y, 0, 0 },
            Vector4 { 0, 0, scale.z, 0 },
            Vector4 { 0, 0, 0, 1 }
        );

        const auto inv = Matrix4(
            Vector4 { real_t( 1 ) / scale.x, 0, 0, 0 },
            Vector4 { 0, real_t( 1 ) / scale.y, 0, 0 },
            Vector4 { 0, 0, real_t( 1 ) / scale.z, 0 },
            Vector4 { 0, 0, 0, 1 }
        );

        return Transformation { m, inv, Transformation::Contents::SCALING };
    }

    [[nodiscard]] inline constexpr Transformation scale( real_t x, real_t y, real_t z ) noexcept
    {
        return scale( Vector3 { x, y, z } );
    }

    [[nodiscard]] inline constexpr Transformation scale( real_t x ) noexcept
    {
        return scale( Vector3 { x, x, x } );
    }

    [[nodiscard]] inline constexpr bool hasScale( const Transformation &t ) noexcept
    {
        return t.contents & Transformation::Contents::SCALING;
    }

}

#endif
