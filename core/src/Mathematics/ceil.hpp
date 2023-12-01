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

#ifndef CRIMILD_MATHEMATICS_CEIL_
#define CRIMILD_MATHEMATICS_CEIL_

#include "Mathematics/Traits.hpp"
#include "Mathematics/tupleBuilder.hpp"
#include "Mathematics/tupleComponents.hpp"

namespace crimild {

    template< concepts::Arithmetic T >
    [[nodiscard]] inline constexpr T ceil( T x ) noexcept
    {
        if constexpr ( traits::isHighPrecision< T >() ) {
            // High precision floating-point should use the standard implementation
            // which is not constexpr.
            return std::ceil( x );
        } else {
            // Use int64 to avoid precision errors when casting float values
            const int64_t i = static_cast< int64_t >( x );
            return static_cast< T >( x > i ? i + 1 : i );
        }
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto ceil( const Tuple2< Derived, T > &t ) noexcept
    {
        return Derived< T > {
            ceil( t.x ),
            ceil( t.y ),
        };
    }

    template< typename T >
    [[nodiscard, deprecated]] inline constexpr auto ceil( const Point2Impl< T > &t ) noexcept
    {
        return Point2Impl< T > {
            ceil( t.x ),
            ceil( t.y ),
        };
    }

    template< typename T >
    [[nodiscard, deprecated]] inline constexpr auto ceil( const Vector3Impl< T > &t ) noexcept
    {
        return Vector3Impl< T > {
            ceil( t.x ),
            ceil( t.y ),
            ceil( t.z )
        };
    }

    template< typename T >
    [[nodiscard, deprecated]] inline constexpr auto ceil( const Normal3Impl< T > &t ) noexcept
    {
        return Normal3Impl< T > {
            ceil( t.x ),
            ceil( t.y ),
            ceil( t.z )
        };
    }

    template< typename T >
    [[nodiscard, deprecated]] inline constexpr auto ceil( const Point3Impl< T > &t ) noexcept
    {
        return Point3Impl< T > {
            ceil( t.x ),
            ceil( t.y ),
            ceil( t.z )
        };
    }

    template< typename T >
    [[nodiscard, deprecated]] inline constexpr auto ceil( const Vector4Impl< T > &t ) noexcept
    {
        return Vector4Impl< T > {
            ceil( t.x ),
            ceil( t.y ),
            ceil( t.z ),
            ceil( t.w )
        };
    }

}

#endif
