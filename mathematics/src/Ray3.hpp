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

#ifndef CRIMILD_MATHEMATICS_RAY_3_
#define CRIMILD_MATHEMATICS_RAY_3_

#include "Point3.hpp"
#include "Vector3.hpp"

namespace crimild {

    /**
     * @brief A ray in 3D space
     *
     * A semi-infinte line specified by its origin and a direction.
     *
     * Rays are represented using the default precision (real_t) for its
     * members, since there is no need for non-floating-points rays.
     */
    struct Ray3 {
        constexpr Ray3( void ) = default;

        constexpr explicit Ray3( const Point3 &origin, const Vector3 &direction ) noexcept
            : origin( origin ),
              direction( direction )
        {
            // no-op
        }

        ~Ray3( void ) = default;

        /**
         * @name Origin and direction
         *
         * Both origin and direction are public variables for convenience.
         */
        //@{

        Point3 origin;
        Vector3 direction;

        //@}

        [[nodiscard]] inline constexpr bool operator==( const Ray3 &other ) const noexcept
        {
            return origin == other.origin && direction == other.direction;
        }

        [[nodiscard]] inline constexpr bool operator!=( const Ray3 &other ) const noexcept
        {
            return !( *this == other );
        }

        /**
         * @brief Evaluates the ray at a given value
         *
         * @code
         * r(t) = o + t * d    // 0 <= t < Inf
         * @encode
         */
        [[nodiscard]] inline constexpr Point3 operator()( real_t t ) const noexcept
        {
            return origin + t * direction;
        }
    };

    [[nodiscard]] inline constexpr const Point3 &origin( const Ray3 &r ) noexcept
    {
        return r.origin;
    }

    [[nodiscard]] inline constexpr const Vector3 &direction( const Ray3 &r ) noexcept
    {
        return r.direction;
    }

}

#endif
