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

#include "Vector.hpp"

namespace crimild {

	/**
		\brief A segment between two points
	 */
	template< crimild::Size SIZE, typename PRECISION >
	class LineSegment {
	public:
		using VectorImpl = Vector< SIZE, PRECISION >;

	public:
		LineSegment( void )
		{

		}

		LineSegment( const VectorImpl &origin, const Vector3f &destination )
			: _origin( origin ),
			  _destination( destination )
		{

		}

		~LineSegment( void )
		{

		}

		void setOrigin( const VectorImpl &origin ) { _origin = origin; }
		inline VectorImpl &getOrigin( void ) { return _origin; }
		inline const VectorImpl &getOrigin( void ) const { return _origin; }

		void setDestination( const VectorImpl &destination ) { _destination = destination; }
		inline VectorImpl &getDestination( void ) { return _destination; }
		inline const VectorImpl &getDestination( void ) const { return _destination; }

		/**
			\brief Classify a point
		*/
		char whichSide( const VectorImpl &p, const VectorImpl &normal ) const
		{
			// compute a plane of the half-space
			const auto n = normal ^ ( _destination - _origin );
			const auto c = -( n * _origin );

			// compute signed distance to plane
			const auto d = ( n * p ) + c;

			if ( d > 0 ) {
				// right
				return 1;
			}
			else if ( d < 0 ) {
				// left
				return -1;
			}

			// the point is on the line
			return 0;
		}

		// Projects a given point into the line segment
		VectorImpl project( const VectorImpl &P ) const
		{
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
		}

		// Projects a given line segment into this one
		LineSegment project( const LineSegment &l ) const
		{
			return LineSegment( project( l.getOrigin() ), project( l.getDestination() ) );
		}

	private:
		VectorImpl _origin;
		VectorImpl _destination;
	};

	template< crimild::Size SIZE, typename PRECISION >
	std::ostream &operator<<( std::ostream &out, const LineSegment< SIZE, PRECISION > &l )
	{
		out << std::setiosflags( std::ios::fixed | std::ios::showpoint  )
			<< std::setprecision( 10 );
		out << "[" << l.getOrigin() << ", " << l.getDestination() << "]";
		return out;
	}

	using LineSegment2i = LineSegment< 2, crimild::Int32 >;
	using LineSegment3i = LineSegment< 3, crimild::Int32 >;

	using LineSegment2f = LineSegment< 2, crimild::Real32 >;
	using LineSegment3f = LineSegment< 3, crimild::Real32 >;

	using LineSegment2d = LineSegment< 2, crimild::Real64 >;
	using LineSegment3d = LineSegment< 3, crimild::Real64 >;

}

#endif

