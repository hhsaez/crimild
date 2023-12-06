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

#ifndef CRIMILD_MATHEMATICS_FRUSTUM_
#define CRIMILD_MATHEMATICS_FRUSTUM_

#include "trigonometry.hpp"

#include <array>
#include <iomanip>
#include <iostream>

namespace crimild {

    class Frustum {
    private:
        enum {
            FRUSTUM_R_MIN = 0,
            FRUSTUM_R_MAX = 1,
            FRUSTUM_U_MIN = 2,
            FRUSTUM_U_MAX = 3,
            FRUSTUM_D_MIN = 4,
            FRUSTUM_D_MAX = 5,
        };

    public:
        constexpr Frustum( void ) noexcept = default;

        constexpr Frustum( Real rMin, Real rMax, Real uMin, Real uMax, Real dMin, Real dMax ) noexcept
            : m_extents( { rMin, rMax, uMin, uMax, dMin, dMax } )
        {
        }

        Frustum( Real verticalFoV, Real aspect, Real dMin, Real dMax ) noexcept
        {
            const auto halfAngleRadians = radians( 0.5f * verticalFoV );

            m_extents[ FRUSTUM_U_MAX ] = dMin * tan( halfAngleRadians );
            m_extents[ FRUSTUM_R_MAX ] = aspect * m_extents[ FRUSTUM_U_MAX ];
            m_extents[ FRUSTUM_U_MIN ] = -m_extents[ FRUSTUM_U_MAX ];
            m_extents[ FRUSTUM_R_MIN ] = -m_extents[ FRUSTUM_R_MAX ];
            m_extents[ FRUSTUM_D_MIN ] = dMin;
            m_extents[ FRUSTUM_D_MAX ] = dMax;
        }

        constexpr Frustum( const Frustum &other ) noexcept
            : m_extents( other.m_extents )
        {
        }

        constexpr Frustum( Frustum &&other ) noexcept
            : m_extents( std::move( other.m_extents ) )
        {
        }

        ~Frustum( void ) noexcept = default;

        constexpr Frustum &operator=( const Frustum &other ) noexcept
        {
            m_extents = other.m_extents;
            return *this;
        }

        constexpr Frustum &operator=( Frustum &&other ) noexcept
        {
            m_extents = std::move( other.m_extents );
            return *this;
        }

        [[nodiscard]] inline constexpr Real getRMin( void ) const noexcept { return m_extents[ 0 ]; }
        [[nodiscard]] inline constexpr Real getRMax( void ) const noexcept { return m_extents[ 1 ]; }
        [[nodiscard]] inline constexpr Real getUMin( void ) const noexcept { return m_extents[ 2 ]; }
        [[nodiscard]] inline constexpr Real getUMax( void ) const noexcept { return m_extents[ 3 ]; }
        [[nodiscard]] inline constexpr Real getDMin( void ) const noexcept { return m_extents[ 4 ]; }
        [[nodiscard]] inline constexpr Real getDMax( void ) const noexcept { return m_extents[ 5 ]; }

        friend std::ostream &operator<<( std::ostream &out, const Frustum &f ) noexcept
        {
            out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
                << std::setprecision( 10 )
                << "[D = (" << f.getDMin() << ", " << f.getDMax() << "), "
                << "R = (" << f.getRMin() << ", " << f.getRMax() << "), "
                << "U = (" << f.getUMin() << ", " << f.getUMax() << ")]";
            return out;
        }

    private:
        std::array< Real, 6 > m_extents = { 0 };
    };

}

#endif
