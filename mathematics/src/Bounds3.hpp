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

#ifndef CRIMILD_CORE_MATHEMATICS_BOUNDS_3_
#define CRIMILD_CORE_MATHEMATICS_BOUNDS_3_

#include "Point3.hpp"

namespace crimild {

    /**
     * \brief An axis aligned 3D bounding box
     *
     * By default, a Bounds3 instance is created with an invalid value,
     * violating the invariant that min <= max. This represents an empty
     * box, ensuring that any operations on it (i.e. union) yield the
     * correct result.
     */
    template< ArithmeticType T >
    struct Bounds3Impl {
        Point3Impl< T > min = Point3Impl< T >::Constants::POSITIVE_INFINITY;
        Point3Impl< T > max = Point3Impl< T >::Constants::NEGATIVE_INFINITY;

        [[nodiscard]] inline constexpr const auto &operator[]( size_t i ) const noexcept
        {
            return i == 0 ? min : max;
        }

        [[nodiscard]] inline auto &operator[]( size_t i ) noexcept
        {
            return i == 0 ? min : max;
        }

        [[nodiscard]] inline constexpr bool operator==( const Bounds3Impl &other ) const noexcept
        {
            return min == other.min && max == other.max;
        }

        [[nodiscard]] inline constexpr bool operator!=( const Bounds3Impl &other ) const noexcept
        {
            return !( *this == other );
        }
    };

    using Bounds3 = Bounds3Impl< real_t >;
    using Bounds3f = Bounds3Impl< float >;
    using Bounds3d = Bounds3Impl< double >;

}

#endif
