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

#ifndef CRIMILD_MATHEMATICS_IO_
#define CRIMILD_MATHEMATICS_IO_

#include "Bounds3.hpp"
#include "ColorRGB.hpp"
#include "ColorRGBA.hpp"
#include "LineSegment3.hpp"
#include "Matrix4.hpp"
#include "Plane3.hpp"
#include "Quaternion.hpp"
#include "Ray3.hpp"
#include "Transformation.hpp"
#include "Tuple2.hpp"
#include "Tuple3.hpp"
#include "Tuple4.hpp"
#include "distance.hpp"
#include "normal.hpp"

#include <iomanip>
#include <iostream>

namespace crimild {

    template< template< ArithmeticType > class Derived, ArithmeticType T >
    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Tuple2< Derived, T > &u ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 6 );
        out << "(" << u.x << ", " << u.y << ")";
        return out;
    }

    template< template< ArithmeticType > class Derived, ArithmeticType T >
    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Tuple3< Derived, T > &u ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 6 );
        out << "(" << u.x << ", " << u.y << ", " << u.z << ")";
        return out;
    }

    template< template< ArithmeticType > class Derived, ArithmeticType T >
    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Tuple4< Derived, T > &u ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 6 );
        out << "(" << u.x << ", " << u.y << ", " << u.z << ", " << u.w << ")";
        return out;
    }

    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Ray3 &R ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 6 );
        out << "[" << origin( R ) << ", " << direction( R ) << "]";
        return out;
    }

    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const LineSegment3 &l ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 10 );
        out << "[" << origin( l ) << ", " << destination( l ) << "]";
        return out;
    }

    template< typename T >
    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const ColorRGBImpl< T > &c ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 6 );
        out << "(" << c.r << ", " << c.g << ", " << c.b << ")";
        return out;
    }

    template< typename T >
    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const ColorRGBAImpl< T > &c ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 6 );
        out << "(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
        return out;
    }

    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Plane3 &p ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 10 )
            << "[" << normal( p ) << ", " << distance( p ) << "]";
        return out;
    }

    template< typename T >
    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Matrix4Impl< T > &a ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 6 );
        out << "[" << a[ 0 ] << ", " << a[ 1 ] << ", " << a[ 2 ] << ", " << a[ 3 ] << "]";
        return out;
    }

    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Quaternion &q ) noexcept
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 10 )
            << "[" << q.v << ", " << q.w << "]";
        return out;
    }

    template< ArithmeticType T >
    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Bounds3Impl< T > &B ) noexcept
    {
        out << "[" << B.min << ", " << B.max << "]";
        return out;
    }

    [[maybe_unused]] static std::ostream &operator<<( std::ostream &out, const Transformation &T ) noexcept
    {
        out << "{ T: " << T.translate << ", R: " << T.rotate << ", S: " << T.scale << " }";
        return out;
    }

}

#endif
