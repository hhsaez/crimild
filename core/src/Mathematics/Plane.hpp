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
#include "Vector3.hpp"

namespace crimild {

    /**
		\brief Defines a plane in three-dimensional space

		A plane is represented by a normal vector and a constant
	 */
    class Plane3 {
    public:
        Plane3( void ) noexcept = default;

        constexpr Plane3( const Normal3 &normal, Real constant ) noexcept
            : m_normal( normal ),
              m_constant( constant )
        {
        }

        constexpr Plane3( const Normal3 &normal, const Point3 &point ) noexcept
            : m_normal( normal ),
              m_constant( -dot( normal, Vector3( point ) ) )
        {
        }

        constexpr Plane3( const Point3 &p0, const Point3 &p1, const Point3 &p2 ) noexcept
            : m_normal( normalize( cross( p2 - p1, p0 - p1 ) ) ),
              m_constant( -dot( m_normal, Vector3 { p0.x(), p0.y(), p0.z() } ) )
        {
        }

        constexpr Plane3( const Plane3 &other ) noexcept
            : m_normal( other.m_normal ),
              m_constant( other.m_constant )
        {
        }

        constexpr Plane3( Plane3 &&other ) noexcept
            : m_normal( other.m_normal ),
              m_constant( other.m_constant )
        {
        }

        ~Plane3( void ) = default;

        constexpr Plane3 &operator=( const Plane3 &other ) noexcept
        {
            m_normal = other.m_normal;
            m_constant = other.m_constant;
            return *this;
        }

        constexpr Plane3 &operator=( Plane3 &&other ) noexcept
        {
            m_normal = std::move( other.m_normal );
            m_constant = std::move( other.m_constant );
            return *this;
        }

        [[nodiscard]] constexpr Bool operator==( const Plane3 &other ) const noexcept
        {
            return m_normal == other.m_normal && m_constant == other.m_constant;
        }

        [[nodiscard]] constexpr Bool operator!=( const Plane3 &other ) const noexcept
        {
            return m_normal != other.m_normal || m_constant != other.m_constant;
        }

        inline constexpr const Normal3 &getNormal( void ) const noexcept { return m_normal; }
        inline constexpr Real getConstant( void ) const noexcept { return m_constant; }

        friend std::ostream &operator<<( std::ostream &out, const Plane3 &p ) noexcept
        {
            out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
                << std::setprecision( 10 )
                << "[" << p.getNormal() << ", " << p.getConstant() << "]";
            return out;
        }

    private:
        Normal3 m_normal;
        Real m_constant;
    };

    [[nodiscard]] constexpr Real distanceSigned( const Plane3 &A, const Point3 &P ) noexcept
    {
        return dot( A.getNormal(), Vector3 { P.x(), P.y(), P.z() } ) + A.getConstant();
    }

    [[nodiscard]] constexpr Real distance( const Plane3 &A, const Point3 &P ) noexcept
    {
        return abs( distanceSigned( A, P ) );
    }

    [[nodiscard]] constexpr Char whichSide( const Plane3 &A, const Point3 &P ) noexcept
    {
        const auto d = distanceSigned( A, P );
        if ( d > 0 ) {
            return +1;
        } else if ( d < 0 ) {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] constexpr Point3 project( const Plane3 &A, const Point3 &P ) noexcept
    {
        const auto d = distanceSigned( A, P );
        const auto V = Vector3( d * A.getNormal() );
        return P - V;
    }

}

#endif
