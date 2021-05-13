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

#ifndef CRIMILD_MATHEMATICS_VECTOR_3_
#define CRIMILD_MATHEMATICS_VECTOR_3_

#include "Mathematics/VectorImpl.hpp"

namespace crimild {

    template< typename T, typename U >
    [[nodiscard]] inline constexpr impl::Vector< T, 3 > cross( const impl::Vector< T, 3 > &u, const impl::Vector< U, 3 > &v ) noexcept
    {
        return impl::Vector< T, 3 > {
            u.y() * v.z() - u.z() * v.y(),
            u.z() * v.x() - u.x() * v.z(),
            u.x() * v.y() - u.y() * v.x(),
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Vector< T, 3 > permute( const impl::Vector< T, 3 > &u, Int x, Int y, Int z ) noexcept
    {
        return impl::Vector< T, 3 > {
            u[ x ],
            u[ y ],
            u[ z ],
        };
    }

    /**
       \brief Creates a orthonormal coordinate system from a vector

       \remarks The input vector v1 is assumed to be already normalized.
     */
    template< typename T >
    void orthonormalBasis( const impl::Vector< T, 3 > &v1, impl::Vector< T, 3 > &v2, impl::Vector< T, 3 > &v3 ) noexcept
    {
        if ( abs( v1.x() ) > abs( v1.y() ) ) {
            v2 = normalize( impl::Vector< T, 3 > { -v1.z(), 0, v1.x() } );
        } else {
            v2 = normalize( impl::Vector< T, 3 > { 0, v1.z(), -v1.y() } );
        }
        v3 = cross( v1, v2 );
    }

    using Vector3 = impl::Vector< Real, 3 >;
    using Vector3f = impl::Vector< Real32, 3 >;
    using Vector3d = impl::Vector< Real64, 3 >;
    using Vector3i = impl::Vector< Int32, 3 >;
    using Vector3ui = impl::Vector< UInt32, 3 >;

}

#endif
