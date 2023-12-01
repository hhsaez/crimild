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

#include "Mathematics/Bounds3.hpp"

#include "Mathematics/Bounds3_bisect.hpp"
#include "Mathematics/Bounds3_centroid.hpp"
#include "Mathematics/Bounds3_combine.hpp"
#include "Mathematics/Bounds3_diagonal.hpp"
#include "Mathematics/Bounds3_easing.hpp"
#include "Mathematics/Bounds3_equality.hpp"
#include "Mathematics/Bounds3_expand.hpp"
#include "Mathematics/Bounds3_inside.hpp"
#include "Mathematics/Bounds3_intersect.hpp"
#include "Mathematics/Bounds3_io.hpp"
#include "Mathematics/Bounds3_isNaN.hpp"
#include "Mathematics/Bounds3_max.hpp"
#include "Mathematics/Bounds3_min.hpp"
#include "Mathematics/Bounds3_overlaps.hpp"
#include "Mathematics/Bounds3_surfaceArea.hpp"
#include "Mathematics/Bounds3_volume.hpp"
#include "Mathematics/Point3_isInfinity.hpp"
#include "Mathematics/intersect.hpp"
#include "Mathematics/io.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Bounds3, defaultValue )
{
    constexpr auto B = Bounds3 {};

    EXPECT_TRUE( isInfinity( B.min ) );
    EXPECT_TRUE( isInfinity( B.max ) );
}

TEST( Bounds3, indexing )
{
    constexpr auto B = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };

    EXPECT_EQ( ( Point3 { 1, 2, 3 } ), B[ 0 ] );
    EXPECT_EQ( ( Point3 { 4, 5, 6 } ), B[ 1 ] );
}

TEST( Bounds3, corner )
{
    constexpr auto B = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };

    EXPECT_EQ( ( Point3 { 1, 2, 3 } ), corner( B, 0 ) );
    EXPECT_EQ( ( Point3 { 4, 2, 3 } ), corner( B, 1 ) );
    EXPECT_EQ( ( Point3 { 1, 5, 3 } ), corner( B, 2 ) );
    EXPECT_EQ( ( Point3 { 4, 5, 3 } ), corner( B, 3 ) );
    EXPECT_EQ( ( Point3 { 1, 2, 6 } ), corner( B, 4 ) );
    EXPECT_EQ( ( Point3 { 4, 2, 6 } ), corner( B, 5 ) );
    EXPECT_EQ( ( Point3 { 1, 5, 6 } ), corner( B, 6 ) );
    EXPECT_EQ( ( Point3 { 4, 5, 6 } ), corner( B, 7 ) );
}

TEST( Bounds3, equality )
{
    constexpr auto B0 = Bounds3 {
        Point3 { 1, 2, 3 },
        Point3 { 4, 5, 6 },
    };

    constexpr auto B1 = Bounds3 {
        Point3 { 1, 2, 3 },
        Point3 { 4, 5, 6 },
    };

    constexpr auto B2 = Bounds3 {
        Point3 { 1, 2, 3 },
        Point3 { 8, 9, 10 },
    };

    EXPECT_EQ( B0, B1 );
    EXPECT_NE( B0, B2 );
}

TEST( Bounds3, isNaN )
{
    EXPECT_FALSE(
        crimild::isNaN(
            Bounds3 {
                Point3 { 1, 2, 3 },
                Point3 { 4, 5, 6 },
            }
        )
    );

    EXPECT_TRUE(
        crimild::isNaN(
            Bounds3 {
                Point3 { 1, NAN, 3 },
                Point3 { 4, 5, 6 },
            }
        )
    );

    EXPECT_TRUE(
        crimild::isNaN(
            Bounds3 {
                Point3 { 1, 2, 3 },
                Point3 { NAN, 5, 6 },
            }
        )
    );

    EXPECT_TRUE(
        crimild::isNaN(
            Bounds3 {
                Point3 { 1, NAN, 3 },
                Point3 { NAN, 5, 6 },
            }
        )
    );
}

TEST( Bounds3, min )
{
    constexpr auto B0 = Bounds3 {
        Point3 { 1, 2, 3 },
        Point3 { 4, 5, 6 },
    };

    constexpr auto B1 = Bounds3 {
        Point3 { 7, 8, 9 },
        Point3 { 10, 11, 12 },
    };

    static_assert( Point3 { 1, 2, 3 } == crimild::min( B0 ) );
    static_assert( Point3 { 1, 2, 3 } == crimild::min( B0, B1 ) );

    EXPECT_TRUE( isInfinity( min( Bounds3 {} ) ) );
}

TEST( Bounds3, max )
{
    constexpr auto B0 = Bounds3 {
        Point3 { 1, 2, 3 },
        Point3 { 4, 5, 6 },
    };

    constexpr auto B1 = Bounds3 {
        Point3 { 7, 8, 9 },
        Point3 { 10, 11, 12 },
    };

    static_assert( Point3 { 4, 5, 6 } == max( B0 ) );
    static_assert( Point3 { 10, 11, 12 } == max( B0, B1 ) );

    EXPECT_TRUE( isInfinity( max( Bounds3 {} ) ) );
}

TEST( Bounds3, minDimension )
{
    EXPECT_EQ(
        2,
        minDimension( Bounds3 {
            Point3 { 1, 2, 3 },
            Point3 { 6, 5, 3.5 },
        } )
    );

    EXPECT_EQ(
        2,
        minDimension( Bounds3 {
            Point3 { -1, 2, 3 },
            Point3 { -6, 5, 4 },
        } )
    );
}

TEST( Bounds3, maxDimension )
{
    EXPECT_EQ(
        0,
        maxDimension( Bounds3 {
            Point3 { 1, 2, 3 },
            Point3 { 4, 5, 6 },
        } )
    );

    EXPECT_EQ(
        1,
        maxDimension( Bounds3 {
            Point3 { 1, 2, 3 },
            Point3 { 4, 10, 6 },
        } )
    );

    EXPECT_EQ(
        0,
        maxDimension( Bounds3 {
            Point3 { -1, 2, 3 },
            Point3 { -10, 5, 4 },
        } )
    );
}

TEST( Bounds3, ostream )
{
    const auto B = Bounds3 {
        Point3 { 1, 2, 3 },
        Point3 { 6, 5, 4 },
    };

    std::stringstream ss;
    ss << B;

    EXPECT_EQ( ss.str(), "[(1.000000, 2.000000, 3.000000), (6.000000, 5.000000, 4.000000)]" );
}

TEST( Bounds3, diagonal )
{
    const auto B = Bounds3 {
        Point3 { 1, 2, 3 },
        Point3 { 6, 5, 4 },
    };

    EXPECT_EQ( ( Vector3 { 5, 3, 1 } ), diagonal( B ) );
}

TEST( Bounds3, centroid )
{
    EXPECT_EQ(
        ( Point3 { 0, 0, 0 } ),
        centroid(
            Bounds3 {
                Point3 { -1, -2, -3 },
                Point3 { 1, 2, 3 },
            }
        )
    );

    EXPECT_EQ(
        ( Point3 { 2.5, 3.5, 4.5 } ),
        centroid(
            Bounds3 {
                Point3 { 1, 2, 3 },
                Point3 { 4, 5, 6 },
            }
        )
    );
}

TEST( Bounds3, combine )
{
    constexpr auto B = Bounds3 {};
    constexpr auto P = Point3 { 1, 2, 3 };
    constexpr auto Q = Point3 { 4, 5, 6 };
    constexpr auto R = Point3 { 2, 3, 4 };

    static_assert( Bounds3 { P, P } == combine( B, P ) );
    static_assert( Bounds3 { P, P } == combine( combine( B, P ), P ) );
    static_assert( Bounds3 { P, Q } == combine( combine( B, P ), Q ) );
    static_assert( Bounds3 { P, Q } == combine( combine( B, Q ), P ) );
    static_assert( Bounds3 { P, Q } == combine( combine( combine( B, P ), Q ), R ) );

    constexpr auto B0 = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };
    constexpr auto B1 = Bounds3 { { 2, 3, 4 }, { 7, 8, 9 } };
    constexpr auto B2 = Bounds3 { { 1, 2, 3 }, { 7, 8, 9 } };
    constexpr auto B3 = Bounds3 { { -1, -2, -3 }, { 4, 5, 6 } };
    constexpr auto B4 = Bounds3 { { -1, -2, -3 }, { 7, 8, 9 } };

    static_assert( B0 == combine( B, B0 ) );
    static_assert( B2 == combine( combine( B, B0 ), B1 ) );
    static_assert( B2 == combine( combine( B, B2 ), B0 ) );
    static_assert( B4 == combine( combine( combine( B, B0 ), B2 ), B3 ) );

    {
        constexpr auto B0 = combine( Bounds3 {}, Point3 { 1, -1, -1 } );
        EXPECT_EQ( B0, ( Bounds3 { { 1, -1, -1 }, { 1, -1, -1 } } ) );

        constexpr auto B1 = combine( B0, Point3 { 1, -1, 1 } );
        EXPECT_EQ( B1, ( Bounds3 { { 1, -1, -1 }, { 1, -1, 1 } } ) );

        constexpr auto B2 = combine( B1, Point3 { -1, -1, 1 } );
        EXPECT_EQ( B2, ( Bounds3 { { -1, -1, -1 }, { 1, -1, 1 } } ) );
    }

    EXPECT_TRUE( true );
}

TEST( Bounds3, intersect )
{
    constexpr auto B0 = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };
    constexpr auto B1 = Bounds3 { { 2, 3, 4 }, { 7, 8, 9 } };
    constexpr auto B2 = Bounds3 { { 2, 3, 4 }, { 4, 5, 6 } };

    static_assert( B2 == intersect( B0, B1 ) );

    EXPECT_TRUE( true );
}

TEST( Bounds3, overlaps )
{
    constexpr auto B0 = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };
    constexpr auto B1 = Bounds3 { { 2, 3, 4 }, { 7, 8, 9 } };
    constexpr auto B2 = Bounds3 { { 4, 5, 6 }, { 7, 8, 9 } };
    constexpr auto B3 = Bounds3 { { 5, 6, 7 }, { 7, 8, 9 } };

    static_assert( overlaps( B0, B1 ) );
    static_assert( overlaps( B0, B2 ) );
    static_assert( !overlaps( B0, B3 ) );
    static_assert( overlaps( B2, B3 ) );

    EXPECT_TRUE( true );
}

// This test aims to replicate the overlap checks when splitting
// a cube at different depths, which is required in RT.
TEST( Bounds3, overlaps_cube )
{
    // Boundings for each of the faces of a unit cube
    constexpr auto front = Bounds3 { { -1, -1, 1 }, { 1, 1, 1 } };
    constexpr auto back = Bounds3 { { -1, -1, -1 }, { 1, 1, -1 } };
    constexpr auto left = Bounds3 { { -1, -1, -1 }, { -1, 1, 1 } };
    constexpr auto right = Bounds3 { { 1, -1, -1 }, { 1, 1, 1 } };
    constexpr auto top = Bounds3 { { -1, 1, -1 }, { 1, 1, 1 } };
    constexpr auto bottom = Bounds3 { { -1, -1, -1 }, { 1, -1, 1 } };

    auto testOverlap = []( const auto &B, std::vector< Bounds3 > pass, std::vector< Bounds3 > fail ) {
        for ( const auto &b : pass ) {
            if ( !overlaps( b, B ) ) {
                return false;
            }
        }
        for ( const auto &b : fail ) {
            if ( overlaps( b, B ) ) {
                return false;
            }
        }

        return true;
    };

    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { -1, -1, -1 }, { 1, 1, 1 } },
            { front, back, left, right, top, bottom },
            {}
        )
    );

    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { -1, -1, -1 }, { 0, 1, 1 } },
            { front, back, left, top, bottom },
            { right }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { 0, -1, -1 }, { 1, 1, 1 } },
            { front, back, right, top, bottom },
            { left }
        )
    );

    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { -1, -1, -1 }, { 0, 0, 1 } },
            { front, back, left, bottom },
            { right, top }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { 0, -1, -1 }, { 1, 0, 1 } },
            { front, back, right, bottom },
            { left, top }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { -1, 0, -1 }, { 0, 1, 1 } },
            { front, back, left, top },
            { right, bottom }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { 0, 0, -1 }, { 1, 1, 1 } },
            { front, back, right, top },
            { left, bottom }
        )
    );

    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { -1, -1, -1 }, { 0, 0, 0 } },
            { back, left, bottom },
            { right, top, front }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { 0, -1, -1 }, { 1, 0, 0 } },
            { back, right, bottom },
            { left, top, front }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { -1, 0, -1 }, { 0, 1, 0 } },
            { back, left, top },
            { right, bottom, front }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { 0, 0, -1 }, { 1, 1, 0 } },
            { back, right, top },
            { left, bottom, front }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { -1, -1, 0 }, { 0, 0, 1 } },
            { front, left, bottom },
            { right, top, back }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { 0, -1, 0 }, { 1, 0, 1 } },
            { front, right, bottom },
            { left, top, back }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { -1, 0, 0 }, { 0, 1, 1 } },
            { front, left, top },
            { right, bottom, back }
        )
    );
    EXPECT_TRUE(
        testOverlap(
            Bounds3 { { 0, 0, 0 }, { 1, 1, 1 } },
            { front, right, top },
            { left, bottom, back }
        )
    );
}

TEST( Bounds3, inside )
{
    constexpr auto B = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };
    constexpr auto P0 = Point3 { 1, 2, 3 };
    constexpr auto P1 = Point3 { 2, 3, 4 };
    constexpr auto P2 = Point3 { 0, 2, 3 };

    static_assert( inside( P0, B ) );
    static_assert( inside( P1, B ) );
    static_assert( !inside( P2, B ) );

    static_assert( !insideExclusive( P0, B ) );

    EXPECT_TRUE( true );
}

TEST( Bounds3, expand )
{
    constexpr auto B = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };
    constexpr auto R = Bounds3 { { 0, 1, 2 }, { 5, 6, 7 } };

    static_assert( R == expand( B, Vector3 { 1, 1, 1 } ) );

    EXPECT_TRUE( true );
}

TEST( Bounds3, surfaceArea )
{
    constexpr auto B = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };

    EXPECT_EQ( 54, surfaceArea( B ) );
}

TEST( Bounds3, volume )
{
    constexpr auto B = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };

    EXPECT_EQ( 27, volume( B ) );
}

TEST( Bounds3, easing )
{
    constexpr auto B = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };

    static_assert( lerp( B, Vector3 { 0.5, 0.5, 0.5 } ) == Point3 { 2.5, 3.5, 4.5 } );

    EXPECT_TRUE( true );
}

TEST( Bounds3, bisect )
{
    constexpr auto B = Bounds3 { { 1, 2, 3 }, { 4, 5, 6 } };

    Bounds3 B0, B1;
    Real split;

    {
        bisect( B, 0, split, B0, B1 );

        constexpr auto X0 = Bounds3 { { 1, 2, 3 }, { 2.5, 5, 6 } };
        constexpr auto X1 = Bounds3 { { 2.5, 2, 3 }, { 4, 5, 6 } };

        EXPECT_EQ( Real( 2.5 ), split );
        EXPECT_EQ( X0, B0 );
        EXPECT_EQ( X1, B1 );
    }

    {
        bisect( B, 1, split, B0, B1 );

        constexpr auto X0 = Bounds3 { { 1, 2, 3 }, { 4, 3.5, 6 } };
        constexpr auto X1 = Bounds3 { { 1, 3.5, 3 }, { 4, 5, 6 } };

        EXPECT_EQ( Real( 3.5 ), split );
        EXPECT_EQ( X0, B0 );
        EXPECT_EQ( X1, B1 );
    }
}