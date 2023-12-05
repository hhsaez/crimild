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

#include "Mathematics/Vector3.hpp"
#include "Mathematics/abs.hpp"
#include "Mathematics/normalize.hpp"

namespace crimild {

    /**
       \brief Creates a orthonormal coordinate system from a vector

       \remarks The input vector v1 is assumed to be already normalized.
     */
    template< typename T >
    static constexpr void orthonormalBasis( const Vector3< T > &v1, Vector3< T > &v2, Vector3< T > &v3 ) noexcept
    {
        // TODO: not sure about handedness...
        if ( abs( v1.x ) > abs( v1.y ) ) {
            v2 = normalize( Vector3< T > { -v1.z, 0, v1.x } );
        } else {
            v2 = normalize( Vector3< T > { 0, v1.z, -v1.y } );
        }
        v3 = cross( v1, v2 );
    }

}

#endif
