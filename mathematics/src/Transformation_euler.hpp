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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_EULER_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_EULER_

#include "Matrix4_transpose.hpp"
#include "Transformation.hpp"

namespace crimild {

    [[nodiscard]] inline Transformation euler( radians_t yaw, radians_t pitch, radians_t roll ) noexcept
    {
        const auto cy = cos( yaw );
        const auto sy = sin( yaw );
        const auto cp = cos( pitch );
        const auto sp = sin( pitch );
        const auto cr = cos( roll );
        const auto sr = sin( roll );

        const auto mat = Matrix4(
            Vector4 { cy * cr + sy * sp * sr, sr * cp, -sy * cr + cy * sp * sr, 0 },
            Vector4 { -cy * sr + sy * sp * cr, cr * cp, sr * sy + cy * sp * cr, 0 },
            Vector4 { sy * cp, -sp, cy * cp, 0 },
            Vector4 { 0, 0, 0, 1 }
        );

        return Transformation {
            .mat = mat,
            .invMat = transpose( mat ),
            .contents = Transformation::Contents::ROTATION,
        };
    }

    inline void extractYawPitchRoll( const Transformation &T, real_t &yaw, real_t &pitch, real_t &roll ) noexcept
    {
        const auto &M = T.mat;
        yaw = atan2( M[ 2 ][ 0 ], M[ 2 ][ 2 ] );
        const auto cosYAngle = sqrt( pow( M[ 0 ][ 1 ], real_t( 2 ) ) + pow( M[ 1 ][ 1 ], real_t( 2 ) ) );
        pitch = atan2( -M[ 2 ][ 1 ], cosYAngle );
        const auto sinXAngle = sin( yaw );
        const auto cosXAngle = cos( yaw );
        roll = -atan2( sinXAngle * M[ 1 ][ 2 ] - cosXAngle * M[ 1 ][ 0 ], cosXAngle * M[ 0 ][ 0 ] - sinXAngle * M[ 0 ][ 2 ] );
    }

}

#endif
