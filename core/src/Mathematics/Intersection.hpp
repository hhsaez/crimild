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

#ifndef CRIMILD_MATHEMATICS_INTERSECTION_
#define CRIMILD_MATHEMATICS_INTERSECTION_

#include "Mathematics/LineSegment3.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Plane3.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/distance.hpp"
#include "Mathematics/pow.hpp"
#include "Numeric.hpp"
#include "Ray3.hpp"
#include "Root.hpp"
#include "Sphere.hpp"
#include "Vector3.hpp"

namespace crimild {

    [[nodiscard]] static constexpr Real findIntersection( const Sphere &S, const Ray3 &R, Real lowerBound = numbers::EPSILON, Real upperBound = numbers::POSITIVE_INFINITY ) noexcept
    {
        const auto centerDiff = origin( R ) - center( S );

        const auto a = lengthSquared( direction( R ) );
        const auto b = Real( 2 ) * dot( centerDiff, direction( R ) );
        const auto c = lengthSquared( centerDiff ) - pow( radius( S ), 2 );

        Real t0 = 0;
        Real t1 = 0;
        if ( findRoots( a, b, c, t0, t1 ) == 0 ) {
            return -1;
        }

        if ( t0 >= lowerBound && t0 < upperBound ) {
            return t0;
        }

        if ( t1 >= lowerBound && t1 < upperBound ) {
            return t1;
        }

        return -1;
    }

    [[nodiscard]] static constexpr Bool checkIntersection( const Sphere &S, const Ray3 &R ) noexcept
    {
        return findIntersection( S, R ) >= 0;
    }

#if 0

    class Intersection {
    public:
        /**
			\brief Test for intersection between a sphere and a ray
		 */
        template< typename T >
        static bool test( const Sphere< T > &sphere, const Ray< 3, T > &ray )
        {
            return find( sphere, ray ) >= 0;
        }

        /**
			\brief Find the intersection between a sphere and a ray
			\return The intersection time (-1 if they do not intersect)
		 */
        template< typename T >
        static T find( const Sphere< T > &sphere,
                       const Ray< 3, T > &ray,
                       T lowerBound = Numeric< T >::ZERO_TOLERANCE,
                       T upperBound = std::numeric_limits< T >::max() )
        {
            Vector< 3, T > centerDiff( ray.getOrigin() - sphere.getCenter() );
            T a = ray.getDirection().getSquaredMagnitude();
            T b = 2 * ( centerDiff * ray.getDirection() );
            T c = centerDiff.getSquaredMagnitude() - ( sphere.getRadius() * sphere.getRadius() );

            T t0, t1;
            if ( Root::compute( a, b, c, t0, t1 ) == 0 ) {
                return -1;
            }

            if ( t0 < upperBound && t0 > lowerBound ) {
                return t0;
            }

            if ( t1 < upperBound && t1 > lowerBound ) {
                return t1;
            }

            return -1;
        }

        /**
			\brief Test for intersection between two spheres
			\returns true if spheres intersect
		 */
        template< typename T >
        static bool test( const Sphere< T > &sphere1, const Sphere< T > &sphere2 )
        {
            T centerDiffSqr = ( sphere1.getCenter() - sphere2.getCenter() ).getSquaredMagnitude();
            T combinedRadius = ( sphere1.getRadius() + sphere2.getRadius() );
            return ( centerDiffSqr < ( combinedRadius * combinedRadius ) );
        }

        /**
			\brief Find the intersection between two spheres
			\todo This method is not implemented yet
		 */
        template< typename T >
        static T find( const Sphere< T > &sphere1, const Sphere< T > &sphere2 )
        {
            return -1;
        }

        /**
			\brief Test for intersection between a sphere and a ray
		 */
        template< typename T >
        static bool test( const Plane< 3, T > &plane, const Ray< 3, T > &ray )
        {
            if ( ( plane.getNormal() * ray.getDirection() ) == 0 ) {
                return false;
            }

            return true;
        }

        /**
			\brief Find the intersection between a plane and a ray
			\return The intersection time (-1 if they do not intersect)
		 */
        template< typename T >
        static T find( const Plane< 3, T > &plane, const Ray< 3, T > &ray )
        {
            T nv = plane.getNormal() * ray.getDirection();
            if ( nv != 0 ) {
                return ( ( -plane.getNormal() * ray.getOrigin() ) + plane.getConstant() ) / nv;
            } else {
                return -1;
            }
        }

        /**
			\brief Ray-Triangle intersection test

			Test for intersection between a ray and a triangle given by three points
		 */
        template< typename T >
        static bool test( const Ray< 3, T > &ray, const Vector< 3, T > &p0, const Vector< 3, T > &p1, const Vector< 3, T > &p2 )
        {
            Vector< 3, T > edge1 = p1 - p0;
            Vector< 3, T > edge2 = p2 - p0;
            Vector< 3, T > triNormal = edge1 ^ edge2;
            if ( ( triNormal * ray.getDirection() ) == 0 ) {
                return false;
            }

            return true;
        }

        /**
			\brief Intersection between three planes
		 */
        template< typename T >
        static Vector< 3, T > find( const Plane< 3, T > &plane1, const Plane< 3, T > &plane2, const Plane< 3, T > &plane3 )
        {
            return Vector< 3, T >( 0, 0, 0 );
        }

        /**
			\brief Ray-Triangle intersection

			Find the intersection between a ray and a triangle given by three points
		 */
        template< typename T >
        static T find( const Ray< 3, T > &ray, const Vector< 3, T > &p0, const Vector< 3, T > &p1, const Vector< 3, T > &p2 )
        {
            return -1;
        }

        template< typename T >
        static bool test( const Vector< 3, T > &min, const Vector< 3, T > &max, const Ray< 3, T > &ray )
        {
            double tx1 = ( min[ 0 ] - ray.getOrigin()[ 0 ] ) / ray.getDirection()[ 0 ];
            double tx2 = ( max[ 0 ] - ray.getOrigin()[ 0 ] ) / ray.getDirection()[ 0 ];

            double tmin = Numericf::min( tx1, tx2 );
            double tmax = Numericf::max( tx1, tx2 );

            double ty1 = ( min[ 1 ] - ray.getOrigin()[ 1 ] ) / ray.getDirection()[ 1 ];
            double ty2 = ( max[ 1 ] - ray.getOrigin()[ 1 ] ) / ray.getDirection()[ 1 ];

            tmin = Numericf::max( tmin, Numericf::min( ty1, ty2 ) );
            tmax = Numericf::min( tmax, Numericf::max( ty1, ty2 ) );

            double tz1 = ( min[ 2 ] - ray.getOrigin()[ 2 ] ) / ray.getDirection()[ 2 ];
            double tz2 = ( max[ 2 ] - ray.getOrigin()[ 2 ] ) / ray.getDirection()[ 2 ];

            tmin = Numericf::max( tmin, Numericf::min( tz1, tz2 ) );
            tmax = Numericf::min( tmax, Numericf::max( tz1, tz2 ) );

            return tmax >= Numericf::max( 0.0, tmin );
        }

        /**
			\brief Finds intersection between two line segments
		*/
        template< crimild::Size SIZE, typename PRECISION >
        static Vector< SIZE, PRECISION > find( const LineSegment< SIZE, PRECISION > &a, const LineSegment< SIZE, PRECISION > &b )
        {
            auto da = a.getDestination() - a.getOrigin();
            auto db = b.getDestination() - b.getOrigin();
            auto dc = b.getOrigin() - a.getOrigin();

            if ( dc * ( da ^ db ) != 0 ) {
                return Vector< SIZE, PRECISION >::ZERO;
            }

            auto dab = da ^ db;

            auto s = ( ( dc ^ db ) * dab ) / ( dab * dab );
            if ( s < 0.0 && s > 1.0 ) {
                return Vector< SIZE, PRECISION >::ZERO;
            }

            return a.getOrigin() + s * da;
        }
    };

#endif

}

#endif
