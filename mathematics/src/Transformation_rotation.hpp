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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_ROTATION_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_ROTATION_

#include "Transformation.hpp"

namespace crimild {

    // \remarks All rotation matrices are calculated based on right-handed coordinate systems

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationX( radians_t angle ) noexcept
    {
        const auto m = Matrix4 {
            { 1, 0, 0, 0 },
            { 0, cos( angle ), sin( angle ), 0 },
            { 0, -sin( angle ), cos( angle ), 0 },
            { 0, 0, 0, 1 },
        };

        const auto inv = Matrix4 {
            { 1, 0, 0, 0 },
            { 0, cos( -angle ), sin( -angle ), 0 },
            { 0, -sin( -angle ), cos( -angle ), 0 },
            { 0, 0, 0, 1 },
        };

        return Transformation { m, inv, Transformation::Contents::ROTATION };
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationY( radians_t angle ) noexcept
    {
        const auto m = Matrix4 {
            { cos( angle ), 0, -sin( angle ), 0 },
            { 0, 1, 0, 0 },
            { sin( angle ), 0, cos( angle ), 0 },
            { 0, 0, 0, 1 },
        };

        const auto inv = Matrix4 {
            { cos( -angle ), 0, -sin( -angle ), 0 },
            { 0, 1, 0, 0 },
            { sin( -angle ), 0, cos( -angle ), 0 },
            { 0, 0, 0, 1 },
        };

        return Transformation { m, inv, Transformation::Contents::ROTATION };
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationZ( radians_t angle ) noexcept
    {
        const auto m = Matrix4 {
            { cos( angle ), sin( angle ), 0, 0 },
            { -sin( angle ), cos( angle ), 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 },
        };

        const auto inv = Matrix4 {
            { cos( -angle ), sin( -angle ), 0, 0 },
            { -sin( -angle ), cos( -angle ), 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 },
        };

        return Transformation { m, inv, Transformation::Contents::ROTATION };
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotation( const Vector3 &axis, radians_t angle ) noexcept
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
        const auto oneMinusCosTheta = real_t( 1 ) - cosTheta;
        const auto oneMinusSinTheta = real_t( 1 ) - sinTheta;

        // TODO: I think I need to transpose this matrix in order to make it work with right-hand coordinate systems
        const auto m = Matrix4 {
            {
                xx * oneMinusCosTheta + cosTheta,
                xy * oneMinusCosTheta + z * sinTheta,
                xz * oneMinusCosTheta - y * sinTheta,
                0,
            },
            {
                xy * oneMinusCosTheta - z * sinTheta,
                yy * oneMinusCosTheta + cosTheta,
                yz * oneMinusCosTheta + x * sinTheta,
                0,
            },
            {
                xz * oneMinusCosTheta + y * sinTheta,
                yz * oneMinusCosTheta - x * sinTheta,
                zz * oneMinusCosTheta + cosTheta,
                0,
            },
            {
                0,
                0,
                0,
                1,
            }
        };

        // cos(-theta) == cos(theta)
        // sin(-theta) == -sin(theta)
        const auto inv = Matrix4 {
            {
                xx * oneMinusCosTheta + cosTheta,
                xy * oneMinusCosTheta - z * sinTheta,
                xz * oneMinusCosTheta + y * sinTheta,
                0,
            },
            {
                xy * oneMinusCosTheta + z * sinTheta,
                yy * oneMinusCosTheta + cosTheta,
                yz * oneMinusCosTheta - x * sinTheta,
                0,
            },
            {
                xz * oneMinusCosTheta - y * sinTheta,
                yz * oneMinusCosTheta + x * sinTheta,
                zz * oneMinusCosTheta + cosTheta,
                0,
            },
            {
                0,
                0,
                0,
                1,
            },
        };

        return Transformation { m, inv, Transformation::Contents::ROTATION };
    }

    [[nodiscard]] inline constexpr bool hasRotation( const Transformation &t ) noexcept
    {
        return t.contents & Transformation::Contents::ROTATION;
    }

}

#endif
