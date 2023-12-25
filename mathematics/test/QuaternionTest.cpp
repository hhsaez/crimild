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

#include "Quaternion.hpp"

#include "conjugate.hpp"
#include "inverse.hpp"
#include "io.hpp"
#include "isEqual.hpp"
#include "length.hpp"
#include "normalize.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( Quaternion, identity )
{
    constexpr Quaternion q;
    static_assert( q.v == Vector3::Constants::ZERO );
    static_assert( q.w == 1 );
    EXPECT_TRUE( true );
}

TEST( Quaternion, construction )
{
    constexpr Quaternion q( 1, 2, 3, 4 );

    static_assert( isEqual( q.v, Vector3 { 1, 2, 3 } ) );
    static_assert( isEqual( q.w, 4 ) );

    EXPECT_TRUE( true );
}

TEST( Quaternion, construction_from_point )
{
    constexpr Point3 p( 1, 2, 3 );
    constexpr Quaternion q( p );
    static_assert( q == Quaternion( 1, 2, 3, 1 ) );

    EXPECT_TRUE( true );
}

TEST( Quaternion, equality )
{
    constexpr Quaternion q( 1, 2, 3, 4 );
    constexpr Quaternion r( 5, 6, 7, 8 );
    constexpr Quaternion s( 1, 2, 3, 4 );

    static_assert( q != r );
    static_assert( q == s );

    EXPECT_TRUE( true );
}

TEST( Quaternion, negation )
{
    constexpr Quaternion q( 1, 2, 3, 4 );
    static_assert( -q == Quaternion( -1, -2, -3, -4 ) );

    EXPECT_TRUE( true );
}

TEST( Quaternion, addition )
{
    constexpr Quaternion q( 1, 2, 3, 4 );
    constexpr Quaternion r( 5, 6, 7, 8 );

    static_assert( q + r == Quaternion( 6, 8, 10, 12 ) );

    EXPECT_TRUE( true );
}

TEST( Quaternion, multiplication )
{
    static_assert(
        isEqual(
            Quaternion( 0, 0, 0, 1 ) * Quaternion( 0, 1, 0, 0 ),
            Quaternion( 0, 1, 0, 0 )
        )
    );

    constexpr Quaternion q( 1, 2, 3, 4 );
    constexpr Quaternion I;
    static_assert( isEqual( q * I, q ) );
    static_assert( isEqual( length( I * I ), 1 ) );

    static_assert(
        isEqual(
            Quaternion( 0.0, 0.7071, 0.0, 0.7071 ) * Quaternion( 0.7071, 0.0, 0.7071, 0.0 ),
            Quaternion( 0.999981, 0, 0, 0 )
        )
    );

    EXPECT_TRUE( true );
}

TEST( Quaternion, conjugate )
{
    constexpr Quaternion q( 1, 2, 3, 4 );
    constexpr Quaternion p( 5, 6, 7, 8 );
    constexpr Quaternion r = conjugate( q );

    static_assert( r == Quaternion( -1, -2, -3, 4 ) );
    static_assert( conjugate( conjugate( q ) ) == q );
    static_assert( conjugate( r ) == q );
    static_assert( conjugate( q + p ) == ( conjugate( q ) + conjugate( p ) ) );
    static_assert( conjugate( q * p ) == ( conjugate( p ) * conjugate( q ) ) );

    EXPECT_TRUE( true );
}

TEST( Quaternion, length )
{
    constexpr Quaternion I;
    static_assert( isEqual( length( I ), 1 ) );

    constexpr Quaternion q( 1, 2, 3, 4 );
    constexpr Quaternion p( 5, 6, 7, 8 );

    static_assert( isEqual( length2( q ), 30.0f ) );
    static_assert( isEqual( length( q ), crimild::sqrt( 30.0f ) ) );
    static_assert( isEqual( length( q ), length( conjugate( q ) ) ) );

    static_assert(
        isEqual(
            length( Quaternion( -0.5, 0.75, -0.25, 0.5 ) ),
            1.06066
        )
    );

    EXPECT_TRUE( true );
}

TEST( Quaternion, normalization )
{
    constexpr Quaternion q( 1, 2, 3, 4 );
    static_assert( isEqual( length( normalize( q ) ), 1 ) );

    constexpr Quaternion r;
    static_assert( isEqual( normalize( r ), r ) );

    static_assert(
        isEqual(
            normalize( Quaternion( -0.5, 0.75, -0.25, 0.5 ) ),
            Quaternion( -0.471405, 0.707107, -0.235702, 0.471405 )
        )
    );

    EXPECT_TRUE( true );
}

TEST( Quaternion, inverse )
{
    constexpr Quaternion q( 1, 2, 3, 4 );
    constexpr Quaternion r = inverse( q );
    constexpr Quaternion I;

    static_assert( isEqual( q * r, I ) );
    static_assert( isEqual( inverse( I ), I ) );

    static_assert(
        isEqual(
            inverse( Quaternion( -0.5, 0.75, -0.25, 0.5 ) ),
            Quaternion( 0.444444, -0.666667, 0.222222, 0.444444 )
        )
    );

    EXPECT_TRUE( true );
}

TEST( Quaternion, matrix_conversion )
{
    constexpr auto q = normalize( Quaternion( -0.5, 0.75, -0.25, 0.5 ) );
    constexpr auto M = Matrix4( q );
    constexpr auto r = Quaternion( M );

    static_assert( isEqual( length( q ), 1 ) );
    static_assert( isEqual( length( r ), 1 ) );
    static_assert( isEqual( q, r ) );

    EXPECT_TRUE( true );
}
