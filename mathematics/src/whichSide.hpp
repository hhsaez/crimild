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

#ifndef CRIMILD_MATHEMATICS_WHICH_SIDE_
#define CRIMILD_MATHEMATICS_WHICH_SIDE_

#include "LineSegment3.hpp"
#include "distance.hpp"

namespace crimild {

    template< typename T >
    [[nodiscard]] constexpr Char whichSide( const LineSegment3Impl< T > &l, const Point3< T > &p, const Normal3< T > &normal ) noexcept
    {
        assert( false && "TODO" );

#if 0
        // compute a plane of the half-space
        const auto n = normal ^ ( _destination - _origin );
        const auto c = -( n * _origin );

        // compute signed distance to plane
        const auto d = ( n * p ) + c;

        if ( d > 0 ) {
            // right
            return 1;
        } else if ( d < 0 ) {
            // left
            return -1;
        }
#endif

        // the point is on the line
        return 0;
    }

    [[nodiscard]] constexpr Char whichSide( const Plane3 &A, const Point3f &P ) noexcept
    {
        const auto d = distanceSigned( A, P );
        if ( d > 0 ) {
            return +1;
        } else if ( d < 0 ) {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr char whichSide( const Plane3 &P, const Sphere &S ) noexcept
    {
        const auto &C = center( S );
        const auto R = radius( S );
        const auto d = distance( P, C );
        if ( d < -R ) {
            return -1; // behind
        } else if ( d > R ) {
            return +1; // front
        }

        return 0; // intersecting
    }

}

#endif
