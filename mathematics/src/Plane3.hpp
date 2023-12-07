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

#ifndef CRIMILD_CORE_MATHEMATICS_PLANE_
#define CRIMILD_CORE_MATHEMATICS_PLANE_

#include "Normal3.hpp"
#include "Point3.hpp"
#include "isEqual.hpp"

namespace crimild {

    /**
        \brief Defines a plane in three-dimensional space

        A plane is represented by a normal vector and a constant

        The default plane is the XZ-plane
    */
    struct Plane3 {
        Normal3f n = Normal3f { 0, 1, 0 };
        real_t d = 0;

        [[nodiscard]] inline constexpr bool operator==( const Plane3 &other ) const noexcept
        {
            return isEqual( this->n, other.n ) && isEqual( this->d, other.d );
        }

        [[nodiscard]] inline constexpr bool operator!=( const Plane3 &other ) const noexcept
        {
            return !isEqual( this->n, other.n ) || !isEqual( this->d, other.d );
        }
    };

    [[nodiscard]] inline constexpr const Normal3f &normal( const Plane3 &p ) noexcept
    {
        return p.n;
    }
    [[nodiscard]] inline constexpr real_t distance( const Plane3 &p ) noexcept
    {
        return p.d;
    }

}

#endif
