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

#ifndef CRIMILD_FOUNDATION_MAP_
#define CRIMILD_FOUNDATION_MAP_

#include <map>
#include <functional>

namespace crimild {

	/**
	   \deprecated Use crimild::containers::Map instead
	 */
    template< typename KEY_TYPE, typename VALUE_TYPE >
    class Map {
    public:
        Map( void ) { }
        Map( const Map &other ) : _map( other ) { }
        ~Map( void ) { }

        Map &operator=( const Map &other ) { _map = other; }

        bool isEmpty( void ) const { return _map.size() == 0; }

        unsigned int size( void ) const { return _map.size(); }

        bool find( KEY_TYPE const &key )
        {
            return _map.find( key ) != _map.end();
        }

        void add( KEY_TYPE const &key, VALUE_TYPE &value )
        {
            _map[ key ] = value;
        }

        void add( KEY_TYPE const &key, VALUE_TYPE const &value )
        {
            _map[ key ] = value;
        }

        void remove( KEY_TYPE const &key )
        {
            _map.erase( key );
        }

        VALUE_TYPE &operator[]( KEY_TYPE const &key )
        {
            return _map[ key ];
        }

        const VALUE_TYPE &operator[]( KEY_TYPE const &key ) const
        {
            return _map[ key ];
        }

        void clear( void )
        {
            _map.clear();
        }

        void foreach( std::function< void( KEY_TYPE const &, VALUE_TYPE &, unsigned int ) > callback ) 
        {
            unsigned int i = 0;
            for ( auto &it : _map ) {
                callback( it.first, it.second, i++ );
            }
        };

    private:
        std::map< KEY_TYPE, VALUE_TYPE > _map;
    };

	template< typename KEY_TYPE, typename VALUE_TYPE >
	using ThreadSafeMap = Map< KEY_TYPE, VALUE_TYPE >; //< TODO

}

#endif

