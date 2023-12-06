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

#ifndef CRIMILD_MATHEMATICS_EXPAND_
#define CRIMILD_MATHEMATICS_EXPAND_

#include "Sphere.hpp"

namespace crimild {

    [[nodiscard]] constexpr Sphere expandToContain( const Sphere &S0, const Sphere &S1 ) noexcept
    {
        const auto &C0 = S0.getCenter();
        const auto R0 = S0.getRadius();
        const auto &C1 = S1.getCenter();
        const auto R1 = S1.getRadius();

        const auto centerDiff = C1 - C0;
        const auto lengthSqr = lengthSquared( centerDiff );
        const auto radiusDiff = R1 - R0;
        const auto radiusDiffSqr = radiusDiff * radiusDiff;

        Point3f C;
        auto R = Real( 0 );
        if ( radiusDiffSqr >= lengthSqr ) {
            if ( radiusDiff >= 0 ) {
                C = C1;
                R = R1;
            }
        } else {
            const auto length = sqrt( lengthSqr );
            if ( !isZero( length ) ) {
                const auto coeff = ( length + radiusDiff ) / ( 2.0 * length );
                C = C0 + coeff * centerDiff;
            }

            R = Real( 0.5 ) * ( length + R0 + R1 );
        }

        return Sphere( C, R );
    }

}

#endif
