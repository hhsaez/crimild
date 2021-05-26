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

#include "Matrix4.hpp"

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

        constexpr Frustum( Real verticalFoV, Real aspect, Real dMin, Real dMax ) noexcept
        {
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

    /*
    template< typename PRECISION >
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
        Frustum( void )
        {
        }

        Frustum( PRECISION rMin, PRECISION rMax, PRECISION uMin, PRECISION uMax, PRECISION dMin, PRECISION dMax )
        {
            _data[ FRUSTUM_R_MIN ] = rMin;
            _data[ FRUSTUM_R_MAX ] = rMax;
            _data[ FRUSTUM_U_MIN ] = uMin;
            _data[ FRUSTUM_U_MAX ] = uMax;
            _data[ FRUSTUM_D_MIN ] = dMin;
            _data[ FRUSTUM_D_MAX ] = dMax;
        }

        Frustum( PRECISION fov, PRECISION aspect, PRECISION near, PRECISION far )
        {
            float halfAngleRadians = 0.5f * fov * Numeric< PRECISION >::DEG_TO_RAD;

            _data[ FRUSTUM_U_MAX ] = near * tan( halfAngleRadians );
            _data[ FRUSTUM_R_MAX ] = aspect * _data[ FRUSTUM_U_MAX ];
            _data[ FRUSTUM_U_MIN ] = -_data[ FRUSTUM_U_MAX ];
            _data[ FRUSTUM_R_MIN ] = -_data[ FRUSTUM_R_MAX ];
            _data[ FRUSTUM_D_MIN ] = near;
            _data[ FRUSTUM_D_MAX ] = far;
        }

        Frustum( const Frustum &frustum )
        {
            memcpy( _data, frustum._data, 6 * sizeof( PRECISION ) );
        }

        ~Frustum( void )
        {
        }

        bool operator==( const Frustum &frustum ) const
        {
            return memcmp( _data, frustum._data, 6 * sizeof( PRECISION ) ) == 0;
        }

        bool operator!=( const Frustum &frustum ) const
        {
            return memcmp( _data, frustum._data, 6 * sizeof( PRECISION ) ) != 0;
        }

        PRECISION getRMin( void ) const { return _data[ FRUSTUM_R_MIN ]; }
        PRECISION getRMax( void ) const { return _data[ FRUSTUM_R_MAX ]; }
        PRECISION getUMin( void ) const { return _data[ FRUSTUM_U_MIN ]; }
        PRECISION getUMax( void ) const { return _data[ FRUSTUM_U_MAX ]; }
        PRECISION getDMin( void ) const { return _data[ FRUSTUM_D_MIN ]; }
        PRECISION getDMax( void ) const { return _data[ FRUSTUM_D_MAX ]; }

        PRECISION computeTanHalfFOV( void ) const
        {
            return getUMax() / getDMin();
        }

        PRECISION computeAspect( void ) const
        {
            return getRMax() / getUMax();
        }

        PRECISION computeLinearDepth( void ) const
        {
            return getDMax() - getDMin();
        }

        Matrix< 4, PRECISION > computeProjectionMatrix( void ) const
        {
            float n = getDMin();
            float f = getDMax();
            float r = getRMax();
            float l = getRMin();
            float t = getUMax();
            float b = getUMin();

            Matrix< 4, PRECISION > projectionMatrix;
            projectionMatrix[ 0 ] = 2 * n / ( r - l );
            projectionMatrix[ 1 ] = 0;
            projectionMatrix[ 2 ] = 0;
            projectionMatrix[ 3 ] = 0;

            projectionMatrix[ 4 ] = 0;
            projectionMatrix[ 5 ] = 2 * n / ( t - b );
            projectionMatrix[ 6 ] = 0;
            projectionMatrix[ 7 ] = 0;

            projectionMatrix[ 8 ] = ( r + l ) / ( r - l );
            projectionMatrix[ 9 ] = ( t + b ) / ( t - b );
            projectionMatrix[ 10 ] = -( f + n ) / ( f - n );
            projectionMatrix[ 11 ] = -1.0f;

            projectionMatrix[ 12 ] = 0;
            projectionMatrix[ 13 ] = 0;
            projectionMatrix[ 14 ] = -( 2.0f * f * n ) / ( f - n );
            projectionMatrix[ 15 ] = 0.0f;

            return projectionMatrix;
        }

        Matrix< 4, PRECISION > computeOrthographicMatrix( void ) const
        {
            float left = getRMin();
            float right = getRMax();
            float bottom = getUMin();
            float top = getUMax();
            float near = getDMin();
            float far = getDMax();

            Matrix< 4, PRECISION > orthographicMatrix;
            orthographicMatrix[ 0 ] = ( 2.0f / ( right - left ) );
            orthographicMatrix[ 1 ] = 0;
            orthographicMatrix[ 2 ] = 0;
            orthographicMatrix[ 3 ] = -( right + left ) / ( right - left );

            orthographicMatrix[ 4 ] = 0;
            orthographicMatrix[ 5 ] = ( 2.0f / ( top - bottom ) );
            orthographicMatrix[ 6 ] = 0;
            orthographicMatrix[ 7 ] = -( top + bottom ) / ( top - bottom );

            orthographicMatrix[ 8 ] = 0;
            orthographicMatrix[ 9 ] = 0;
            orthographicMatrix[ 10 ] = ( -2.0f / ( far - near ) );
            orthographicMatrix[ 11 ] = -( far + near ) / ( far - near );

            orthographicMatrix[ 12 ] = 0;
            orthographicMatrix[ 13 ] = 0;
            orthographicMatrix[ 14 ] = 0;
            orthographicMatrix[ 15 ] = 1;

            return orthographicMatrix;
        }

    private:
        PRECISION _data[ 6 ];
    };

    template< typename PRECISION >
    std::ostream &operator<<( std::ostream &out, const Frustum< PRECISION > &f )
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 10 )
            << "[D = (" << f.getDMin() << ", " << f.getDMax() << "), "
            << "R = (" << f.getRMin() << ", " << f.getRMax() << "), "
            << "U = (" << f.getUMin() << ", " << f.getUMax() << ")]";
        return out;
    }

    typedef Frustum< float > Frustumf;
    typedef Frustum< double > Frustumd;

    */

}

#endif
