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

#ifndef CRIMILD_NAVIGATION_CELL
#define CRIMILD_NAVIGATION_CELL

#include "Coding/Codable.hpp"
#include "Mathematics/LineSegment.hpp"
#include "Mathematics/Plane.hpp"
#include "NavigationCellEdge.hpp"

namespace crimild {

    namespace navigation {

        class NavigationCell : public coding::Codable {
            CRIMILD_IMPLEMENT_RTTI( crimild::navigation::NavigationCell )

        public:
            explicit NavigationCell( const Vector3f &v0, const Vector3f &v1, const Vector3f &v2 );
            virtual ~NavigationCell( void );

            inline Vector3f getVertex( crimild::Size index ) { return _vertices[ index ]; }

            inline const Vector3f getCenter( void ) const { return _center; }

            inline const Vector3f getNormal( void ) const { return _normal; }

            inline const Plane3 getPlane( void ) const { return _plane; }

            void addEdge( NavigationCellEdgePtr const &e ) { _edges.push_back( e ); }

            void foreachEdge( std::function< void( NavigationCellEdgePtr const &e ) > const &callback )
            {
                for ( auto &e : _edges )
                    callback( e );
            }

            bool containsPoint( const Vector3f &p ) const;

        private:
            Vector3f _vertices[ 3 ];
            Vector3f _normal;
            Vector3f _center;
            Plane3 _plane;

            std::vector< NavigationCellEdgePtr > _edges;

        public:
            enum class ClassificationResult {
                INSIDE,
                OUTSIDE,
                NONE
            };

            ClassificationResult classifyPath( const LineSegment3 &motionPath, Vector3f &intersectionPoint, NavigationCellEdge **intersectionEdge );

            Vector3f snapPoint( const Vector3f &point );
        };

        using NavigationCellPtr = SharedPointer< NavigationCell >;

    }

}

#endif
