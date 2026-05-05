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

#ifndef CRIMILD_MATHEMATICS_EULER_
#define CRIMILD_MATHEMATICS_EULER_

#include "Quaternion.hpp"

namespace crimild {

    // from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    [[nodiscard]] inline Quaternion euler( radians_t yaw, radians_t pitch, radians_t roll ) noexcept
    {
        real_t t0 = cos( roll * 0.5 );
        real_t t1 = sin( roll * 0.5 );
        real_t t2 = cos( pitch * 0.5 );
        real_t t3 = sin( pitch * 0.5 );
        real_t t4 = cos( yaw * 0.5 );
        real_t t5 = sin( yaw * 0.5 );
        return Quaternion {
            {
                t0 * t3 * t4 - t1 * t2 * t5,
                t0 * t2 * t5 + t1 * t3 * t4,
                t1 * t2 * t4 - t0 * t3 * t5,
            },
            t0 * t2 * t4 + t1 * t3 * t5,
        };
    }

    // from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    [[nodiscard]] inline auto toEuler( const Quaternion &q ) noexcept
    {

        real_t x = q.v.x;
        real_t y = q.v.y;
        real_t z = q.v.z;
        real_t w = q.w;

        real_t ysqr = y * y;

        // roll (x-axis rotation)
        real_t t0 = +2.0f * ( w * x + y * z );
        real_t t1 = +1.0f - 2.0f * ( x * x + ysqr );
        real_t roll = std::atan2( t0, t1 );

        // pitch (y-axis rotation)
        real_t t2 = +2.0f * ( w * y - z * x );
        t2 = t2 > 1.0f ? 1.0f : t2;
        t2 = t2 < -1.0f ? -1.0f : t2;
        real_t pitch = std::asin( t2 );

        // yaw (z-axis rotation)
        real_t t3 = +2.0f * ( w * z + x * y );
        real_t t4 = +1.0f - 2.0f * ( ysqr + z * z );
        real_t yaw = std::atan2( t3, t4 );

        return std::array< real_t, 3 > { pitch, yaw, roll };
    }

}

#endif
