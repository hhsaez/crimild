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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_OPS_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_OPS_

#include "Mathematics/Matrix4Ops.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/cross.hpp"
#include "Mathematics/dot.hpp"
#include "Mathematics/normalize.hpp"

[[nodiscard]] constexpr crimild::Transformation operator*( const crimild::Transformation &t0, const crimild::Transformation &t1 ) noexcept
{
    return crimild::Transformation {
        .mat = t0.mat * t1.mat,
        .invMat = t0.invMat * t1.invMat,
        .contents = t0.contents | t1.contents,
    };
}

namespace crimild {

    [[nodiscard]] inline constexpr Bool isIdentity( const Transformation &t ) noexcept
    {
        return t.contents == Transformation::Contents::IDENTITY;
    }

    [[nodiscard]] inline constexpr Transformation inverse( const Transformation &t ) noexcept
    {
        return Transformation { t.invMat, t.mat, t.contents };
    }

    [[nodiscard]] static constexpr Transformation translation( const Vector3 &delta ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            1, 0, 0, delta.x,
            0, 1, 0, delta.y,
            0, 0, 1, delta.z,
            0, 0, 0, 1,
        };

        const auto inv = Matrix4 {
            1, 0, 0, -delta.x,
            0, 1, 0, -delta.y,
            0, 0, 1, -delta.z,
            0, 0, 0, 1,
        };
        // clang-format on

        return Transformation { m, inv, Transformation::Contents::TRANSLATION };
    }

    [[nodiscard]] inline constexpr Transformation translation( Real x, Real y, Real z ) noexcept
    {
        return translation( Vector3 { x, y, z } );
    }

    [[nodiscard]] inline constexpr Bool hasTranslation( const Transformation &t ) noexcept
    {
        return t.contents & Transformation::Contents::TRANSLATION;
    }

    [[nodiscard]] static constexpr Transformation scale( const Vector3 &scale ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1,
        };

        const auto inv = Matrix4 {
            Real( 1 ) / scale.x , 0, 0, 0,
            0, Real( 1 ) / scale.y, 0, 0,
            0, 0, Real( 1 ) / scale.z, 0,
            0, 0, 0, 1,
        };
        // clang-format on

        return Transformation { m, inv, Transformation::Contents::SCALING };
    }

    [[nodiscard]] inline constexpr Transformation scale( Real x, Real y, Real z ) noexcept
    {
        return scale( Vector3 { x, y, z } );
    }

    [[nodiscard]] inline constexpr Transformation scale( Real x ) noexcept
    {
        return scale( Vector3 { x, x, x } );
    }

    [[nodiscard]] inline constexpr Bool hasScale( const Transformation &t ) noexcept
    {
        return t.contents & Transformation::Contents::SCALING;
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationX( Radians angle ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            1, 0, 0, 0,
            0, cos( angle ), -sin( angle ), 0,
            0, sin( angle ), cos( angle ), 0,
            0, 0, 0, 1
        };

        const auto inv = Matrix4 {
            1, 0, 0, 0,
            0, cos( -angle ), -sin( -angle ), 0,
            0, sin( -angle ), cos( -angle ), 0,
            0, 0, 0, 1
        };
        // clang-format on

        return Transformation { m, inv, Transformation::Contents::ROTATION };
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationY( Radians angle ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            cos( angle ), 0, sin( angle ), 0,
            0, 1, 0, 0,
            -sin( angle ), 0, cos( angle ), 0,
            0, 0, 0, 1
        };

        const auto inv = Matrix4 {
            cos( -angle ), 0, sin( -angle ), 0,
            0, 1, 0, 0,
            -sin( -angle ), 0, cos( -angle ), 0,
            0, 0, 0, 1
        };
        // clang-format on

        return Transformation { m, inv, Transformation::Contents::ROTATION };
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationZ( Radians angle ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            cos( angle ), -sin( angle ), 0, 0,
            sin( angle ), cos( angle ), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        const auto inv = Matrix4 {
            cos( -angle ), -sin( -angle ), 0, 0,
            sin( -angle ), cos( -angle ), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        // clang-format on

        return Transformation { m, inv, Transformation::Contents::ROTATION };
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotation( const Vector3 &axis, Radians angle ) noexcept
    {
        const auto x = axis.x;
        const auto y = axis.y;
        const auto z = axis.z;
        const auto xx = axis.x * axis.x;
        const auto xy = axis.x * axis.y;
        const auto xz = axis.x * axis.z;
        const auto yy = axis.y * axis.y;
        const auto yz = axis.y * axis.z;
        const auto zz = axis.z * axis.z;
        const auto cosTheta = cos( angle );
        const auto sinTheta = sin( angle );
        const auto oneMinusCosTheta = Real( 1 ) - cosTheta;
        const auto oneMinusSinTheta = Real( 1 ) - sinTheta;

        // clang-format off
        const auto m = Matrix4 {
            xx * oneMinusCosTheta + cosTheta,
            xy * oneMinusCosTheta - z * sinTheta,
            xz * oneMinusCosTheta + y * sinTheta,
            0,

            xy * oneMinusCosTheta + z * sinTheta,
            yy * oneMinusCosTheta + cosTheta,
            yz * oneMinusCosTheta - x * sinTheta,
            0,

            xz * oneMinusCosTheta - y * sinTheta,
            yz * oneMinusCosTheta + x * sinTheta,
            zz * oneMinusCosTheta + cosTheta,
            0,

            0,
            0,
            0,
            1,
        };

        // cos(-theta) == cos(theta)
        // sin(-theta) == -sin(theta)
        const auto inv = Matrix4 {
            xx * oneMinusCosTheta + cosTheta,
            xy * oneMinusCosTheta + z * sinTheta,
            xz * oneMinusCosTheta - y * sinTheta,
            0,

            xy * oneMinusCosTheta - z * sinTheta,
            yy * oneMinusCosTheta + cosTheta,
            yz * oneMinusCosTheta + x * sinTheta,
            0,

            xz * oneMinusCosTheta + y * sinTheta,
            yz * oneMinusCosTheta - x * sinTheta,
            zz * oneMinusCosTheta + cosTheta,
            0,

            0,
            0,
            0,
            1,
        };
        // clang-format on

        return Transformation { m, inv, Transformation::Contents::ROTATION };
    }

    [[nodiscard]] inline constexpr Bool hasRotation( const Transformation &t ) noexcept
    {
        return t.contents & Transformation::Contents::ROTATION;
    }

    [[nodiscard]] static constexpr Transformation lookAt( const Point3 &pos, const Point3 &target, const Vector3 &up ) noexcept
    {
        const auto T = Vector3 { target.x, target.y, target.z };
        const auto F = normalize( target - pos );
        const auto R = normalize( cross( normalize( up ), F ) );
        const auto U = cross( F, R );

        // clang-format off
        const auto cameraToWorld = Matrix4 {
            R.x, R.y, R.z, -dot( T, R ),
            U.x, U.y, U.z, -dot( T, U ),
            F.x, F.y, F.z, -dot( T, F ),
            0, 0, 0, 1,
        };
        // clang-format on

        return Transformation { cameraToWorld, inverse( cameraToWorld ), Transformation::Contents::ROTATION | Transformation::Contents::TRANSLATION };
    }
}

#endif
