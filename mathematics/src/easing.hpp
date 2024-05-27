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

#ifndef CRIMILD_MATHEMATICS_EASING_
#define CRIMILD_MATHEMATICS_EASING_

#include "Bounds3.hpp"
#include "Transformation.hpp"
#include "max.hpp"
#include "min.hpp"
#include "trigonometry.hpp"
#include "types.hpp"

namespace crimild {

    template< typename T >
    [[nodiscard]] inline constexpr T lerp( const T &start, const T &end, real_t t ) noexcept
    {
        return ( real_t( 1 ) - t ) * start + t * end;
    }

    template< ArithmeticType T >
    [[nodiscard]] inline constexpr auto lerp( const Bounds3Impl< T > &B, const Vector3Impl< T > &t ) noexcept
    {
        return Point3 {
            lerp( min( B ).x, max( B ).x, t.x ),
            lerp( min( B ).y, max( B ).y, t.y ),
            lerp( min( B ).z, max( B ).z, t.z ),
        };
    }

    /**
     * \brief Calculate spherical liner interpolation for two quaternions
     *
     * \param q0 Original quaternion. Must be unit length
     * \param q1 Destination quaternion. Must be unit length
     *
     * \remarks This interpolation requires the use of quaternions only
     */
    [[nodiscard]] static Quaternion slerp( const Quaternion &a, const Quaternion &b, real_t t ) noexcept
    {
        real_t cosTheta = ( a.w * b.w ) + dot( a.v, b.v );
        Quaternion b1 = b;
        if ( cosTheta < 0.0 ) {
            // fix rotation for big angles
            b1 = -b;
            cosTheta = a.w * b1.w + dot( a.v, b1.v );
        }

        real_t w1, w2;
        real_t theta = crimild::acos( cosTheta );
        real_t sinTheta = crimild::sin( theta );

        if ( sinTheta > 0.0001 ) {
            w1 = crimild::sin( ( 1.0 - t ) * theta ) / sinTheta;
            w2 = crimild::sin( t * theta ) / sinTheta;
        } else {
            w1 = 1.0 - t;
            w2 = t;
        }

        Quaternion result = a * w1 + b1 * w2;
        return normalize( result );
    }

    // Interpolate two transformations
    // This is a very complex operation and should not be used frequently.
    [[nodiscard]] constexpr Transformation lerp( const Transformation &start, const Transformation &end, real_t t ) noexcept
    {
        if ( t <= 0 ) {
            return start;
        }

        if ( t >= 1 ) {
            return end;
        }

        return Transformation {
            .translate = lerp( start.translate, end.translate, t ),
            .rotate = slerp( start.rotate, end.rotate, t ),
            .scale = lerp( start.scale, end.scale, t ),
        };
    }

}

#endif
