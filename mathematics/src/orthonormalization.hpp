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

#ifndef CRIMILD_MATHEMATICS_ORTHONORMALIZATION_
#define CRIMILD_MATHEMATICS_ORTHONORMALIZATION_

#include "Vector3.hpp"
#include "abs.hpp"
#include "isEqual.hpp"
#include "normalize.hpp"
#include "pow.hpp"
#include "sign.hpp"

namespace crimild {

    /**
       \brief Creates a orthonormal coordinate system from a vector

       Find two normalized vectors given a normalized 3D one such that all three vectors
       are mutually perpendicular. Such vectors can be expressed as follows:

       v2 = [
        (1 - vx^2) / (1 + vz),
        (vx * vy) / (1 + vz),
        -vx
       ]

       v3 = [
        (vx * vy) / (1 + vz),
        (1 - vy^2) / (1 + vz),
        -vy
       ]

       Handles special case where v1.z ~= -1, in which case there is a loss of accuracy when `1 / (1 + v1.z)` is calculated.

       \remarks The input vector v1 is assumed to be already normalized.
     */
    template< typename T >
    static constexpr void orthonormalBasis( const Vector3Impl< T > &v1, Vector3Impl< T > &v2, Vector3Impl< T > &v3 ) noexcept
    {
        assert( isEqual( lengthSquared( v1 ), 1 ) );
        const real_t s = sign( v1.z );
        const real_t a = -1 / ( s + v1.z );
        const real_t b = v1.x * v1.y * a;
        v2 = Vector3Impl< T >( 1 + s * pow( v1.x, 2 ) * a, s * b, -s * v1.x );
        v3 = Vector3Impl< T >( b, s + pow( v1.y, 2 ) * a, -v1.y );
    }

}

#endif
