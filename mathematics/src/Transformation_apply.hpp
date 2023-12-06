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

#include "Matrix4_transpose.hpp"
#include "Point3.hpp"
#include "Transformation.hpp"
#include "Transformation_isIdentity.hpp"
#include "Transformation_operators.hpp"
#include "Vector3.hpp"
#include "normalize.hpp"
#include "swizzle.hpp"

namespace crimild {

    /**
     *  \brief Checks if a given Transformation swaps the coordinate system handedness.
     *
     *  Certain transformations change the handedness of the coordinate system. For example, using negative
     *  scale values. Some operations need to know this in order to compute the right values for normals, for example.
     */
    [[nodiscard]] inline constexpr Bool swapsHandedness( const Transformation &T ) noexcept
    {
        // Compute the determinant of the upper 3x3 matrix
        Real det = T.mat[ 0 ][ 0 ] * ( T.mat[ 1 ][ 1 ] * T.mat[ 2 ][ 2 ] - T.mat[ 2 ][ 1 ] * T.mat[ 1 ][ 2 ] )
                   - T.mat[ 1 ][ 0 ] * ( T.mat[ 0 ][ 1 ] * T.mat[ 2 ][ 2 ] - T.mat[ 2 ][ 1 ] * T.mat[ 0 ][ 2 ] )
                   + T.mat[ 2 ][ 0 ] * ( T.mat[ 0 ][ 1 ] * T.mat[ 1 ][ 2 ] - T.mat[ 1 ][ 1 ] * T.mat[ 0 ][ 2 ] );

        // If the determinant is negative, then the transformation swaps handedness
        return det < 0;
    }

    [[nodiscard]] constexpr Point3f Transformation::operator()( const Point3f &p ) const noexcept
    {
        if ( isIdentity( *this ) ) {
            return p;
        }

        const auto V = mat * Vector4f( p );
        return Point3f( V );
    }

    [[nodiscard]] constexpr Vector3f Transformation::operator()( const Vector3f &v ) const noexcept
    {
        if ( isIdentity( *this ) ) {
            return v;
        }

        const auto v0 = Vector4f( v );
        const auto v1 = mat * v0;
        return Vector3 {
            v1.x,
            v1.y,
            v1.z,
        };
    }

    [[nodiscard]] constexpr Normal3f Transformation::operator()( const Normal3f &N ) const noexcept
    {
        if ( isIdentity( *this ) ) {
            return N;
        }

        const auto V = Vector4f( N );
        const auto R = transpose( invMat ) * V;
        const auto ret = Normal3 {
            R.x,
            R.y,
            R.z,
        };

        // If handedness is swapped by this transformation, then we need to flip the normal.
        return swapsHandedness( *this ) ? -ret : ret;
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

    [[nodiscard]] inline constexpr Point3f location( const Transformation &t ) noexcept
    {
        return t( Point3f::Constants::ZERO );
    }

    [[nodiscard]] inline constexpr Vector3f right( const Transformation &t ) noexcept
    {
        return normalize( t( Vector3f::Constants::RIGHT ) );
    }

    [[nodiscard]] inline constexpr Vector3f up( const Transformation &t ) noexcept
    {
        return normalize( t( Vector3f::Constants::UP ) );
    }

    [[nodiscard]] inline constexpr Vector3f forward( const Transformation &t ) noexcept
    {
        return normalize( t( Vector3f::Constants::FORWARD ) );
    }

}

#endif
