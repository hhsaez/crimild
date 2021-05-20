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

#ifndef CRIMILD_MATHEMATICS_SPHERE_
#define CRIMILD_MATHEMATICS_SPHERE_

#include "Plane.hpp"
#include "Point3.hpp"

namespace crimild {

    class Sphere {
    public:
        constexpr Sphere( void ) noexcept = default;

        constexpr Sphere( const Point3 &center, Real radius ) noexcept
            : m_center( center ),
              m_radius( radius )
        {
        }

        constexpr Sphere( const Sphere &other ) noexcept
            : m_center( other.m_center ),
              m_radius( other.m_radius )
        {
        }

        constexpr Sphere( Sphere &&other ) noexcept
            : m_center( std::move( other.m_center ) ),
              m_radius( std::move( other.m_radius ) )
        {
        }

        ~Sphere( void ) = default;

        constexpr Sphere &operator=( const Sphere &other ) noexcept
        {
            m_center = other.m_center;
            m_radius = other.m_radius;
            return *this;
        }

        constexpr Sphere &operator=( Sphere &&other ) noexcept
        {
            m_center = std::move( other.m_center );
            m_radius = std::move( other.m_radius );
            return *this;
        }

        inline constexpr const Point3 &getCenter( void ) const noexcept { return m_center; }
        inline constexpr Real getRadius( void ) const noexcept { return m_radius; }

    private:
        Point3 m_center = Point3::Constants::ZERO;
        Real m_radius = 1.0;
    };

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

        Point3 C;
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

    [[nodiscard]] constexpr char whichSide( const Plane3 &P, const Sphere &S ) noexcept
    {
        const auto &C = S.getCenter();
        const auto R = S.getRadius();
        const auto d = distance( P, C );
        if ( d < -R ) {
            return -1; // behind
        } else if ( d > R ) {
            return +1; // front
        }

        return 0; // intersecting
    }

}

#endif
