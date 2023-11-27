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

#ifndef CRIMILD_MATHEMATICS_CYLINDER_NORMAL_
#define CRIMILD_MATHEMATICS_CYLINDER_NORMAL_

#include "Mathematics/Cylinder.hpp"
#include "Mathematics/Normal3.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_inverse.hpp"
#include "Mathematics/normalize.hpp"
#include "Mathematics/swizzle.hpp"

namespace crimild {

    // Project the point in the XZ plane and normalize
    // TODO(hernan): Take into account the cylinder's center
    [[nodiscard]] inline constexpr Normal3 normal( const Cylinder &C, const Point3 &P ) noexcept
    {
        const Real dist = P.x * P.x + P.z * P.z;
        const Real r = radius( C );
        const Real r2 = r * r;
        const Real h = height( C );

        if ( dist < r2 && P.y >= -h - numbers::EPSILON ) {
            return Normal3 { 0, 1, 0 };
        }

        if ( dist < r2 && P.y <= h + numbers::EPSILON ) {
            return Normal3 { 0, -1, 0 };
        }

        return Normal3 { P.x, 0, P.z };
    }

    [[nodiscard]] inline constexpr Normal3 normal( const Cylinder &C, const Transformation &T, const Point3 &P ) noexcept
    {
        const auto localP = inverse( T )( P );
        return normalize( T( normal( C, localP ) ) );
    }

}

#endif
