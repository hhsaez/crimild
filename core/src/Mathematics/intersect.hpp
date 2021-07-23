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

#ifndef CRIMILD_MATHEMATICS_INTERSECT_
#define CRIMILD_MATHEMATICS_INTERSECT_

#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Sphere.hpp"
#include "Mathematics/dot.hpp"
#include "Mathematics/pow.hpp"
#include "Mathematics/sqrt.hpp"

#include <limits>

namespace crimild {

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Sphere &S, Real &t0, Real &t1 ) noexcept
    {
        const auto CO = origin( R ) - center( S );
        const auto a = dot( direction( R ), direction( R ) );
        const auto b = Real( 2 ) * dot( direction( R ), CO );
        const auto c = dot( CO, CO ) - pow( radius( S ), 2 );

        const auto d = b * b - Real( 4 ) * a * c;

        if ( d < 0 ) {
            return false;
        }

        const auto sqrtD = sqrt( d );

        t0 = ( -b - sqrtD ) / ( 2 * a );
        t1 = ( -b + sqrtD ) / ( 2 * a );

        return true;
    }

}

#endif
