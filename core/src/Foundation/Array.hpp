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

#ifndef CRIMILD_FOUNDATION_ARRAY_
#define CRIMILD_FOUNDATION_ARRAY_

#include "Types.hpp"

#include <vector>
#include <functional>
#include <algorithm>

namespace crimild {

	/**
	   \deprecated Use crimild::collections::Array instead
	 */
    template< typename VALUE_TYPE >
    class Array {
    public:
        Array( void ) { }
        Array( unsigned int size ) : _array( size ) { }
        Array( const Array &other ) : _array( other._array ) { }
        ~Array( void ) { }

        bool isEmpty( void ) const { return size() == 0; }

		/**
		   \deprecated Use getCount() instead
		 */
        unsigned int size( void ) const { return _array.size(); }

		inline crimild::Size getCount( void ) { return _array.size(); }

		inline VALUE_TYPE *getData( void ) { return &_array[ 0 ]; }

		inline const VALUE_TYPE *getData( void ) const { return &_array[ 0 ]; }

        void resize( unsigned int size ) { _array.resize( size ); }

        bool find( const VALUE_TYPE &value ) const
        {
            return _array.find( value ) != _array.end();
        }

        void add( VALUE_TYPE &value )
        {
            _array.push_back( value );
        }

        void add( VALUE_TYPE const &value )
        {
            _array.push_back( value );
        }

        void remove( VALUE_TYPE &value )
        {
            _array.erase( std::remove( _array.begin(), _array.end(), value ), _array.end() );
        }

        VALUE_TYPE &operator[]( crimild::Size index ) { return _array[ index ]; }

        const VALUE_TYPE &operator[]( crimild::Size index ) const { return _array[ index ]; }

        void clear( void )
        {
            _array.clear();
        }

        void foreach( std::function< void( VALUE_TYPE &, unsigned int ) > callback ) 
        {
			if ( getCount() == 0 ) {
				return;
			}

			const auto count = getCount();

			// optimized for loop
			for ( int i = 0; i < count; i++ ) {
                callback( _array[ i ], i );
            }
        };

		void swap( crimild::Size a, crimild::Size b )
		{
			auto t = _array[ a ];
			_array[ a ] = _array[ b ];
			_array[ b ] = t;
		}

    private:
        std::vector< VALUE_TYPE > _array;
    };

	template< typename T >
	using ThreadSafeArray = Array< T >;

}

#endif

