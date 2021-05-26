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

#include "Mathematics/LineSegment3.hpp"

namespace crimild {

    // Projects a given point into the line segment
    template< typename T, Size N >
    [[nodiscard]] constexpr impl::Point< T, N > project( const impl::LineSegment< T, N > &l, const impl::Point< T, N > &P ) noexcept
    {
        assert( false && "TODO" );

#if 0
        const auto A = getOrigin();
        const auto B = getDestination();
        const auto AB = B - A;
        const auto dAB = AB * AB;

        if ( Numericf::isZero( dAB ) ) {
            // same point
            return A;
        }

        const auto AP = P - A;

        const auto t = ( AP * AB ) / dAB;

        if ( t < 0 ) {
            return A;
        }

        if ( t > 1 ) {
            return B;
        }

        return A + t * AB;
#endif

        return impl::Point< T, N > {};
    }

    // Projects a given line segment into this one
    template< typename T, Size N >
    [[nodiscard]] constexpr impl::LineSegment< T, N > project( const impl::LineSegment< T, N > &l0, const impl::LineSegment< T, N > &l1 ) noexcept
    {
        assert( false && "TODO" );
        return impl::LineSegment< T, N > {};
        //return LineSegment( project( l.getOrigin() ), project( l.getDestination() ) );
    }

    [[nodiscard]] constexpr Point3 project( const Plane3 &A, const Point3 &P ) noexcept
    {
        const auto d = distanceSigned( A, P );
        const auto V = Vector3( d * A.getNormal() );
        return P - V;
    }

}

#endif
