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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_

#include "Matrix4.hpp"
#include "Normal3.hpp"
#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Ray3.hpp"
#include "Vector3.hpp"
#include "normalize.hpp"

namespace crimild {

    /**
     * @brief A function from points to points and vectors to vectors
     *
     * There are many ways to represent transformations.
     * In the end, I chose the SRT (actually SQT) one because
     * it's more suited for interpolations and compositions.
     */
    class Transformation {
    public:
        struct Constants;

    public:
        // All members are initialized to the itentity values. This might have some performance
        // impact, but it definitely prevents a lot of logic errors due to un-initialized values.
        Point3 translate = Point3::Constants::ZERO;
        Quaternion rotate = Quaternion::Constants::IDENTITY;
        Vector3 scale = Vector3::Constants::ONE;

        [[nodiscard]] inline constexpr bool operator==( const Transformation &other ) const noexcept
        {
            return translate == other.translate && rotate == other.rotate && scale == other.scale;
        }

        [[nodiscard]] inline constexpr bool operator!=( const Transformation &other ) const noexcept
        {
            return !( *this == other );
        }

        [[nodiscard]] constexpr Point3 operator()( const Point3 &p ) const noexcept
        {
            return Point3( rotate * ( scale * Vector3( p ) ) ) + translate;
        }

        [[nodiscard]] constexpr Vector3 operator()( const Vector3 &v ) const noexcept
        {
            return rotate * ( scale * v );
        }

        friend inline constexpr bool swapsHandedness( const Transformation &T ) noexcept;

        [[nodiscard]] constexpr Normal3 operator()( const Normal3 &n ) const noexcept
        {
            // Normals are not affected by scale, unless the transformation swaps handedness,
            // in which the resulting normal is the inverse of the rotated one.
            const auto ret = Normal3( rotate * Vector3( n ) );
            return swapsHandedness( *this ) ? -ret : ret;
        }

        [[nodiscard]] constexpr Ray3 operator()( const Ray3 &R ) const noexcept
        {
            return Ray3 {
                .origin = ( *this )( R.origin ),
                .direction = ( *this )( R.direction ),
            };
        }

        [[nodiscard]] constexpr Transformation operator()( const Transformation &T ) const noexcept
        {
            return Transformation {
                .translate = ( *this )( T.translate ),
                .rotate = rotate * T.rotate,
                .scale = scale * T.scale,
            };
        }
    };

    struct Transformation::Constants {
        static constexpr auto IDENTITY = Transformation {};
    };

    [[nodiscard]] inline constexpr bool isIdentity( const Transformation &T ) noexcept
    {
        return T == Transformation::Constants::IDENTITY;
    }

    [[nodiscard]] inline constexpr bool hasTranslation( const Transformation &T ) noexcept
    {
        return T.translate != Point3::Constants::ZERO;
    }

    [[nodiscard]] inline constexpr bool hasRotation( const Transformation &T ) noexcept
    {
        return T.rotate != Quaternion::Constants::IDENTITY;
    }

    [[nodiscard]] inline constexpr bool hasScale( const Transformation &T ) noexcept
    {
        return T.scale != Vector3::Constants::ONE;
    }

    /**
     *  \brief Checks if a given Transformation swaps the coordinate system handedness.
     *
     *  Certain transformations change the handedness of the coordinate system. For example, using negative
     *  scale values. Some operations need to know this in order to compute the right values for normals, for example.
     */
    [[nodiscard]] inline constexpr bool swapsHandedness( const Transformation &T ) noexcept
    {
        return ( T.scale.x * T.scale.y * T.scale.z ) < 0;
    }

    [[nodiscard]] inline constexpr Vector3 right( const Transformation &t ) noexcept
    {
        return normalize( t( Vector3::Constants::RIGHT ) );
    }

    [[nodiscard]] inline constexpr Vector3 up( const Transformation &t ) noexcept
    {
        return normalize( t( Vector3::Constants::UP ) );
    }

    [[nodiscard]] inline constexpr Vector3 forward( const Transformation &t ) noexcept
    {
        return normalize( t( Vector3::Constants::FORWARD ) );
    }

    /**
     * @brief Creates a 4x4 Matrix from a Transformation
     */
    template<>
    constexpr Matrix4Impl< real_t >::Matrix4Impl( const Transformation &t ) noexcept
    {
        if ( t == Transformation::Constants::IDENTITY ) {
            *this = Matrix4Impl::Constants::IDENTITY;
            return;
        }

        real_t x = t.rotate.v.x;
        real_t y = t.rotate.v.y;
        real_t z = t.rotate.v.z;
        real_t w = t.rotate.w;
        real_t sx = t.scale.x;
        real_t sy = t.scale.y;
        real_t sz = t.scale.z;

        real_t x2 = x + x;
        real_t y2 = y + y;
        real_t z2 = z + z;

        real_t xx = x * x2;
        real_t xy = x * y2;
        real_t xz = x * z2;
        real_t yy = y * y2;
        real_t yz = y * z2;
        real_t zz = z * z2;
        real_t wx = w * x2;
        real_t wy = w * y2;
        real_t wz = w * z2;

        ( *this )[ 0 ][ 0 ] = sx * ( 1 - ( yy + zz ) );
        ( *this )[ 0 ][ 1 ] = sx * ( xy + wz );
        ( *this )[ 0 ][ 2 ] = sx * ( xz - wy );
        ( *this )[ 0 ][ 3 ] = 0;

        ( *this )[ 1 ][ 0 ] = sy * ( xy - wz );
        ( *this )[ 1 ][ 1 ] = sy * ( 1 - ( xx + zz ) );
        ( *this )[ 1 ][ 2 ] = sy * ( yz + wx );
        ( *this )[ 1 ][ 3 ] = 0;

        ( *this )[ 1 ][ 0 ] = sz * ( xz + wy );
        ( *this )[ 1 ][ 1 ] = sz * ( yz - wx );
        ( *this )[ 1 ][ 2 ] = sz * ( 1 - ( xx + yy ) );
        ( *this )[ 1 ][ 3 ] = 0;

        ( *this )[ 3 ][ 0 ] = t.translate.x;
        ( *this )[ 3 ][ 1 ] = t.translate.y;
        ( *this )[ 3 ][ 2 ] = t.translate.z;
        ( *this )[ 3 ][ 3 ] = 1;
    }

}

#endif
