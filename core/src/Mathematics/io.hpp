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

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/LineSegment3.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Plane3.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Tuple2.hpp"
#include "Mathematics/Tuple3.hpp"
#include "Mathematics/Tuple4.hpp"

#include <iomanip>
#include <iostream>

template< typename T >
static std::ostream &operator<<( std::ostream &out, const crimild::impl::Tuple2< T > &u ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 6 );
    out << "(" << u.x << ", " << u.y << ")";
    return out;
}

template< typename T >
static std::ostream &operator<<( std::ostream &out, const crimild::impl::Tuple3< T > &u ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 6 );
    out << "(" << u.x << ", " << u.y << ", " << u.z << ")";
    return out;
}

template< typename T >
static std::ostream &operator<<( std::ostream &out, const crimild::impl::Tuple4< T > &u ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 6 );
    out << "(" << u.x << ", " << u.y << ", " << u.z << ", " << u.w << ")";
    return out;
}

static std::ostream &operator<<( std::ostream &out, const crimild::Ray3 &R ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 6 );
    out << "[" << crimild::origin( R ) << ", " << crimild::direction( R ) << "]";
    return out;
}

static std::ostream &operator<<( std::ostream &out, const crimild::LineSegment3 &l ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 10 );
    out << "[" << crimild::origin( l ) << ", " << crimild::destination( l ) << "]";
    return out;
}

template< typename T >
static std::ostream &operator<<( std::ostream &out, const crimild::impl::ColorRGB< T > &c ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 6 );
    out << "(" << c.r << ", " << c.g << ", " << c.b << ")";
    return out;
}

template< typename T >
static std::ostream &operator<<( std::ostream &out, const crimild::impl::ColorRGBA< T > &c ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 6 );
    out << "(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
    return out;
}

static std::ostream &operator<<( std::ostream &out, const crimild::Plane3 &p ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 10 )
        << "[" << crimild::normal( p ) << ", " << crimild::distance( p ) << "]";
    return out;
}

template< typename T >
static std::ostream &operator<<( std::ostream &out, const crimild::impl::Matrix4< T > &a ) noexcept
{
    out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
        << std::setprecision( 6 );
    out << "[" << a[ 0 ] << ", " << a[ 1 ] << ", " << a[ 2 ] << ", " << a[ 3 ] << "]";
    return out;
}

#endif