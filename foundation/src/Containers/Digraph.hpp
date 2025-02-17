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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_DIGRAPH_
#define CRIMILD_FOUNDATION_CONTAINERS_DIGRAPH_

#include "Common/Types.hpp"
#include "Containers/Array.hpp"
#include "Containers/List.hpp"
#include "Containers/Map.hpp"
#include "Containers/Queue.hpp"
#include "Containers/Set.hpp"

#include <cassert>

namespace crimild {

    template< typename VERTEX_TYPE >
    class Digraph {
    public:
        virtual ~Digraph( void ) = default;

        inline void hasVertex( VERTEX_TYPE const &v ) const noexcept
        {
            return _adj.contains( v );
        }

        inline void addVertex( VERTEX_TYPE const &v ) noexcept
        {
            _adj.insert( v, Set< VERTEX_TYPE >() );
        }

        inline Size getVertexCount( void ) const noexcept
        {
            return _adj.size();
        }

        void eachVertex( std::function< void( VERTEX_TYPE const & ) > const &callback ) noexcept
        {
            _adj.eachKey( [ callback ]( VERTEX_TYPE const &v ) {
                callback( v );
            } );
        }

        void addEdge( VERTEX_TYPE const &v, VERTEX_TYPE const &w ) noexcept
        {
            if ( !_adj.contains( v ) )
                _adj.insert( v, {} );
            if ( !_adj.contains( w ) )
                _adj.insert( w, {} );

            _adj[ v ].insert( w );
        }

        Size getEdgeCount( void ) const noexcept
        {
            Size count = 0;
            _adj.each( [ &count ]( const VERTEX_TYPE &, const Set< VERTEX_TYPE > &edges ) {
                count += edges.size();
            } );
            return count;
        }

        inline Size getEdgeCount( VERTEX_TYPE const &v ) const noexcept
        {
            return _adj.contains( v ) ? _adj[ v ].size() : 0;
        }

        template< typename Callback >
        void eachEdge( VERTEX_TYPE const &v, Callback callback ) noexcept
        {
            if ( _adj.contains( v ) ) {
                _adj[ v ].each( [ callback ]( VERTEX_TYPE const &w ) {
                    callback( w );
                } );
            }
        }

        Size outDegree( VERTEX_TYPE const &v ) noexcept
        {
            if ( !_adj.contains( v ) ) {
                return 0;
            }

            return _adj[ v ].size();
        }

        Array< VERTEX_TYPE > sort( void ) noexcept
        {
            Array< VERTEX_TYPE > result;

            List< VERTEX_TYPE > frontier;
            List< VERTEX_TYPE > expanded;
            Map< VERTEX_TYPE, Int32 > inCount;

            auto rGraph = reverse();
            eachVertex( [ &rGraph, &inCount, &frontier ]( VERTEX_TYPE const &v ) {
                auto inputs = rGraph.outDegree( v );
                if ( inputs > 0 ) {
                    inCount[ v ] = inputs;
                } else {
                    frontier.add( v );
                }
            } );

            while ( !frontier.empty() ) {
                auto node = frontier.first();
                frontier.remove( node );
                expanded.add( node );
                result.add( node );

                eachEdge( node, [ &expanded, &frontier, &inCount ]( VERTEX_TYPE const &other ) {
                    if ( expanded.contains( other ) || frontier.contains( other ) ) {
                        assert( false && "Graph contains loops" );
                    } else {
                        inCount[ other ] -= 1;
                        if ( inCount[ other ] <= 0 ) {
                            frontier.add( other );
                        }
                    }
                } );
            }

            return result;
        }

        Set< VERTEX_TYPE > connected( VERTEX_TYPE source ) noexcept
        {
            Set< VERTEX_TYPE > result;
            dfs( source, result );
            return result;
        }

        Set< VERTEX_TYPE > connected( Array< VERTEX_TYPE > const &sources ) noexcept
        {
            Set< VERTEX_TYPE > result;
            sources.each( [ this, &result ]( VERTEX_TYPE const &v ) {
                if ( !result.contains( v ) ) {
                    dfs( v, result );
                }
            } );
            return result;
        }

        void add( Digraph &other ) noexcept
        {
            other.eachVertex( [ this, &other ]( VERTEX_TYPE const &v ) {
                other.eachEdge( v, [ this, &v ]( VERTEX_TYPE const &w ) {
                    addEdge( v, w );
                } );
            } );
        }

        Digraph reverse( void ) noexcept
        {
            Digraph r;
            eachVertex( [ this, &r ]( VERTEX_TYPE const &v ) {
                eachEdge( v, [ &r, &v ]( VERTEX_TYPE const &w ) {
                    r.addEdge( w, v );
                } );
            } );

            return r;
        }

    private:
        void dfs( VERTEX_TYPE const &s, Set< VERTEX_TYPE > &r ) noexcept
        {
            Queue< VERTEX_TYPE > frontier;
            frontier.push( s );
            while ( !frontier.empty() ) {
                auto v = frontier.pop();
                eachEdge( v, [ &frontier, &r ]( VERTEX_TYPE const &w ) {
                    if ( !r.contains( w ) && !frontier.contains( w ) ) {
                        r.insert( w );
                        frontier.push( w );
                    }
                } );
            }
        }

    private:
        Map< VERTEX_TYPE, Set< VERTEX_TYPE > > _adj;
    };

}

#endif
