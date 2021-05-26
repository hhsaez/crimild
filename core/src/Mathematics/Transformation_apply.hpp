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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_APPLY_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_APPLY_

#include "Mathematics/Transformation.hpp"
#include "Mathematics/TransformationOps.hpp"
#include "Mathematics/swizzle.hpp"

namespace crimild {

    [[nodiscard]] constexpr Point3 Transformation::operator()( const Point3 &p ) const noexcept
    {
        if ( isIdentity( *this ) ) {
            return p;
        }

        const auto V = mat * vector4( p, Real( 1 ) );
        return point3( xyz( V ) );
    }

    [[nodiscard]] constexpr Vector3 Transformation::operator()( const Vector3 &v ) const noexcept
    {
        if ( isIdentity( *this ) ) {
            return v;
        }

        const auto v0 = vector4( v, Real( 0 ) );
        const auto v1 = mat * v0;
        return Vector3 {
            v1.x,
            v1.y,
            v1.z,
        };
    }

    [[nodiscard]] constexpr Normal3 Transformation::operator()( const Normal3 &N ) const noexcept
    {
        if ( isIdentity( *this ) ) {
            return N;
        }

        const auto V = vector4( N, Real( 0 ) );
        const auto R = transpose( invMat ) * V;
        return Normal3 {
            R.x,
            R.y,
            R.z,
        };
    }

    [[nodiscard]] constexpr Ray3 Transformation::operator()( const Ray3 &R ) const noexcept
    {
        if ( isIdentity( *this ) ) {
            return R;
        }

        return Ray3 {
            ( *this )( origin( R ) ),
            ( *this )( direction( R ) ),
        };
    }

}

#endif
