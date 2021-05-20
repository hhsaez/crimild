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

#ifndef CRIMILD_CORE_MATHEMATICS_LINE_SEGMENT_
#define CRIMILD_CORE_MATHEMATICS_LINE_SEGMENT_

#include "Normal3.hpp"
#include "PointImpl.hpp"

namespace crimild {

    namespace impl {

        /**
           \brief A segment between two points
        */
        template< typename T, Size N >
        class LineSegment {
        private:
            using PointImpl = Point< T, N >;

        public:
            constexpr LineSegment( void ) noexcept = default;

            constexpr LineSegment( const PointImpl &origin, const PointImpl &destination ) noexcept
                : m_origin( origin ),
                  m_destination( destination )
            {
            }

            constexpr LineSegment( const LineSegment &other ) noexcept
                : m_origin( other.m_origin ),
                  m_destination( other.m_destination )
            {
            }

            constexpr LineSegment( LineSegment &&other ) noexcept
                : m_origin( std::move( other.m_origin ) ),
                  m_destination( std::move( other.m_destination ) )
            {
            }

            ~LineSegment( void ) = default;

            constexpr LineSegment &operator=( const LineSegment &other ) noexcept
            {
                m_origin = other.m_origin;
                m_destination = other.m_destination;
                return *this;
            }

            constexpr LineSegment &operator=( LineSegment &&other ) noexcept
            {
                m_origin = std::move( other.m_origin );
                m_destination = std::move( other.m_destination );
                return *this;
            }

            [[nodiscard]] constexpr Bool operator==( const LineSegment &other ) const noexcept
            {
                return m_origin == other.m_origin && m_destination == other.m_destination;
            }

            [[nodiscard]] constexpr Bool operator!=( const LineSegment &other ) const noexcept
            {
                return m_origin != other.m_origin || m_destination != other.m_destination;
            }

            inline constexpr const PointImpl &getOrigin( void ) const noexcept { return m_origin; }
            inline constexpr const PointImpl &getDestination( void ) const noexcept { return m_destination; }

            friend std::ostream &operator<<( std::ostream &out, const LineSegment &l ) noexcept
            {
                out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
                    << std::setprecision( 10 );
                out << "[" << l.getOrigin() << ", " << l.getDestination() << "]";
                return out;
            }

        private:
            PointImpl m_origin;
            PointImpl m_destination;
        };

    }

    using LineSegment2 = impl::LineSegment< Real, 2 >;

    using LineSegment3 = impl::LineSegment< Real, 3 >;

    /**
       \brief Classify a point
    */
    template< typename T, Size N >
    [[nodiscard]] constexpr Char whichSide( const impl::LineSegment< T, N > &l, const impl::Point< T, N > &p, const Normal3 &normal ) noexcept
    {
        assert( false && "TODO" );

#if 0
        // compute a plane of the half-space
        const auto n = normal ^ ( _destination - _origin );
        const auto c = -( n * _origin );

        // compute signed distance to plane
        const auto d = ( n * p ) + c;

        if ( d > 0 ) {
            // right
            return 1;
        } else if ( d < 0 ) {
            // left
            return -1;
        }
#endif

        // the point is on the line
        return 0;
    }

    // Projects a given point into the line segment
    template< typename T, Size N >
    [[nodiscard]] constexpr impl::Point< T, N > project( const impl::LineSegment< T, N > &l, const impl::Point< T, N > &P ) noexcept
    {
        assert( false && "TODO" );

#if 0
        const auto A = getOrigin();
        const auto B = getDestination();
        const auto AB = B - A;
        const auto dAB = AB * AB;

        if ( Numericf::isZero( dAB ) ) {
            // same point
            return A;
        }

        const auto AP = P - A;

        const auto t = ( AP * AB ) / dAB;

        if ( t < 0 ) {
            return A;
        }

        if ( t > 1 ) {
            return B;
        }

        return A + t * AB;
#endif

        return impl::Point< T, N > {};
    }

    // Projects a given line segment into this one
    template< typename T, Size N >
    [[nodiscard]] constexpr impl::LineSegment< T, N > project( const impl::LineSegment< T, N > &l0, const impl::LineSegment< T, N > &l1 ) noexcept
    {
        assert( false && "TODO" );
        return impl::LineSegment< T, N > {};
        //return LineSegment( project( l.getOrigin() ), project( l.getDestination() ) );
    }

}

#endif
