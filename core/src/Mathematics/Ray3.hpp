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

#include "Mathematics/Point3.hpp"
#include "Mathematics/Vector3.hpp"

namespace crimild {

    class Ray3 {
    public:
        constexpr explicit Ray3( const Point3 &origin, const Vector3 &direction ) noexcept
            : m_origin( origin ),
              m_direction( direction )
        {
        }

        constexpr Ray3( const Ray3 &other ) noexcept
            : m_origin( other.m_origin ),
              m_direction( other.m_direction )
        {
        }

        constexpr Ray3( Ray3 &&other ) noexcept
            : m_origin( std::move( other.m_origin ) ),
              m_direction( std::move( other.m_direction ) )
        {
        }

        ~Ray3( void ) = default;

        constexpr inline Ray3 &operator=( const Ray3 &other ) noexcept
        {
            m_origin = other.m_origin;
            m_direction = other.m_direction;
            return *this;
        }

        constexpr inline Ray3 &operator=( Ray3 &&other ) noexcept
        {
            m_origin = std::move( other.m_origin );
            m_direction = std::move( other.m_direction );
            return *this;
        }

        [[nodiscard]] constexpr inline Bool operator==( const Ray3 &other ) const noexcept
        {
            return m_origin == other.m_origin && m_direction == other.m_direction;
        }

        [[nodiscard]] constexpr inline Bool operator!=( const Ray3 &other ) const noexcept
        {
            return m_origin != other.m_origin || m_direction != other.m_direction;
        }

        constexpr inline const Point3 &getOrigin( void ) const noexcept { return m_origin; }
        constexpr inline const Vector3 &getDirection( void ) const noexcept { return m_direction; }

        [[nodiscard]] inline constexpr Point3 operator()( Real t ) const noexcept { return m_origin + t * m_direction; }

        friend inline std::ostream &operator<<( std::ostream &out, const Ray3 &R )
        {
            out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
                << std::setprecision( 6 );
            out << "[" << R.m_origin << ", " << R.m_direction << "]";
            return out;
        }

    private:
        Point3 m_origin;
        Vector3 m_direction;
    };

    [[nodiscard]] constexpr Real distanceSquared( const Ray3 &R, const Point3 &P ) noexcept
    {
        const auto V = P - R.getOrigin();
        const auto d = dot( V, R.getDirection() );
        return ( dot( V, V ) - d * d ) / lengthSquared( R.getDirection() );
    }

    [[nodiscard]] constexpr Real distance( const Ray3 &R, const Point3 &P ) noexcept
    {
        return sqrt( distanceSquared( R, P ) );
    }

}

#endif
