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

#ifndef CRIMILD_MATHEMATICS_PROJECT_
#define CRIMILD_MATHEMATICS_PROJECT_

#include "LineSegment3.hpp"
#include "Plane3.hpp"
#include "distance.hpp"
#include "isZero.hpp"
#include "normal.hpp"
#include "origin.hpp"

namespace crimild {

    // Projects a given point into the line segment
    template< ArithmeticType T >
    [[nodiscard]] constexpr auto project( const LineSegment3 &l, const Point3Impl< T > &P ) noexcept
    {
        const auto A = origin( l );
        const auto B = destination( l );
        const auto AB = B - A;
        const auto dAB = dot( AB, AB );

        if ( isZero( dAB ) ) {
            // same point
            return A;
        }

        const auto AP = P - A;

        const auto t = dot( AP, AB ) / dAB;

        if ( t < 0 ) {
            return A;
        }

        if ( t > 1 ) {
            return B;
        }

        return A + t * AB;
    }

    // Projects a given line segment into this one
    [[nodiscard]] constexpr auto project( const LineSegment3 &l0, const LineSegment3 &l1 ) noexcept
    {
        return LineSegment3 {
            .p0 = project( l0, origin( l1 ) ),
            .p1 = project( l0, destination( l1 ) ),
        };
    }

    [[nodiscard]] constexpr Point3f project( const Plane3 &A, const Point3f &P ) noexcept
    {
        const auto d = distance2( A, P );
        const auto V = Vector3( d * normal( A ) );
        return P - V;
    }

}

#endif
