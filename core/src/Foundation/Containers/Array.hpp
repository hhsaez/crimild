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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_ARRAY_
#define CRIMILD_FOUNDATION_CONTAINERS_ARRAY_

#include "Foundation/Types.hpp"
#include "Foundation/Policies/ThreadingPolicy.hpp"
#include "Mathematics/Numeric.hpp"

#include <functional>
#include <iostream>

namespace crimild {

	namespace containers {

		/**
		   \brief A resizable array implementation
		   
		   \todo Implement index bound checking policy
		   \todo Implement parallel policy
		*/
		template<
		    typename T,
		    class ThreadingPolicy = policies::SingleThreaded
		>
		class Array : public ThreadingPolicy {
		private:
			using LockImpl = typename ThreadingPolicy::Lock;
			
		public:
			using TraverseCallback = std::function< void( T &, crimild::Size ) >;
			using ConstTraverseCallback = std::function< void( const T &, crimild::Size ) >;
			
		public:
			Array( void )
			{
                resize_unsafe( 1 );
			}
			
			explicit Array( crimild::Size size )
			{
				resize_unsafe( size );
				
				_size = _capacity;
			}

			Array( std::initializer_list< T > l )
				: Array( l.size() )
			{
				_size = 0;
				for ( auto e : l ) {
					_elems[ _size++ ] = e;
				}
			}

			Array( const Array &other )
				: Array( other._size )
			{
                for ( crimild::Size i = 0; i < _size; i++ ) {
                    _elems[ i ] = other._elems[ i ];
                }
			}
			
			virtual ~Array( void )
			{
				
			}
            
            Array &operator=( const Array &other )
            {
                LockImpl lock( this );
                
                resize_unsafe( other._capacity );
                _size = other._size;
                for ( crimild::Size i = 0; i < _size; i++ ) {
                    _elems[ i ] = other._elems[ i ];
                }
                
                return *this;
            }

			/**
			   \brief Compare two arrays up to getSize() elements
			 */
			bool operator==( const Array &other ) const
			{
				if ( _size != other._size ) {
					return false;
				}
                
                for ( crimild::Size i = 0; i < _size; i++ ) {
                    if ( _elems[ i ] != other._elems[ i ] ) {
                        return false;
                    }
                }
                
                return true;
			}
			
			inline bool empty( void ) const
			{
				LockImpl lock( this );
				return size_unsafe() == 0;
			}
			
			inline crimild::Size size( void ) const
			{
				LockImpl lock( this );
				return size_unsafe();
			}
            
            inline void clear( void )
            {
                LockImpl lock( this );
                resize_unsafe( 0 );
                _size = 0;
            }
			
			inline T &operator[]( crimild::Size index )
			{
				LockImpl lock( this );
				return _elems[ index ];
			}
			
			inline const T &operator[]( crimild::Size index ) const
			{
				LockImpl lock( this );
				return _elems[ index ];
			}
            
            T *getData( void )
            {
                LockImpl lock( this );
                return &_elems[ 0 ];
            }
            
            const T *getData( void ) const
            {
                LockImpl lock( this );
                return &_elems[ 0 ];
            }
			
			void add( T const &elem )
			{
				LockImpl lock( this );
				if ( _size == _capacity ) {
					resize_unsafe( 2 * _capacity );
				}

				_elems[ _size++ ] = elem;
			}
			
			void remove( const T &elem )
			{
				LockImpl lock( this );

				crimild::Size i = 0;
				
				while ( i < _size && _elems[ i ] != elem ) {
					i++;
				}
				
				if ( i < _size ) {
					removeAt_unsafe( i );
				}
			}
			
			void removeAt( crimild::Size index )
			{
				LockImpl lock( this );
				removeAt_unsafe( index );
			}
			
			void swap( crimild::Size i, crimild::Size j )
			{
				LockImpl lock( this );
				swap_unsafe( i, j );
			}
			
			void resize( crimild::Size capacity )
			{
				LockImpl lock( this );
				resize_unsafe( capacity );
                _size = capacity;
			}

			crimild::Bool contains( const T &e ) const
			{
				LockImpl lock( this );
				for ( crimild::Size i = 0; i < _size; i++ ) {
					if ( _elems[ i ] == e ) {
						return true;
					}
				}
				return false;
			}

			void each( TraverseCallback const &callback )
			{
				LockImpl lock( this );

				// implement a policy to traverse the array by creating a copy if needed
				for ( crimild::Size i = 0; i < _size; i++ ) {
					callback( _elems[ i ], i );
				}
			}

			void each( ConstTraverseCallback const &callback ) const
			{
				LockImpl lock( this );

				// implement a policy to traverse the array by creating a copy if needed
				for ( crimild::Size i = 0; i < _size; i++ ) {
					callback( _elems[ i ], i );
				}
			}

		private:
			crimild::Size size_unsafe( void ) const
			{
				return _size;
			}
			
			void resize_unsafe( crimild::Size capacity )
			{
                capacity = Numeric< crimild::Size >::max( 1, capacity );
				auto elems = std::unique_ptr< T[] >( new T[ capacity ] );
                auto count = Numeric< crimild::Size >::min( capacity, _capacity );
                for ( crimild::Size i = 0; i < count; i++ ) {
                    elems[ i ] = _elems[ i ];
                }
                _capacity = capacity;
				_elems = std::move( elems );
			}

			void swap_unsafe( crimild::Size i, crimild::Size j )
			{
				auto t = _elems[ i ];
				_elems[ i ] = _elems[ j ];
				_elems[ j ] = t;
			}
			
			void removeAt_unsafe( crimild::Size index )
			{
				for ( crimild::Size i = index; i < _size - 1; i++ ) {
					_elems[ i ] = _elems[ i + 1 ];
				}
				_elems[ _size - 1 ] = T(); // avoid loitering
				--_size;
				
				if ( _size > 0 && _size == _capacity / 4 ) {
					// resize the array if needed
					resize_unsafe( _size / 2 );
				}			
			}
			
		private:
			std::unique_ptr< T[] > _elems;
			crimild::Size _size = 0;
			crimild::Size _capacity = 0;
		};

		using ByteArray = Array< crimild::Byte >;
		
	}

}

template<
    typename T,
    class TP
>
std::ostream& operator<<( std::ostream& os, const crimild::containers::Array< T, TP > &array )  
{  
	os << "[";
	array.each( [&os]( const T &a, crimild::Size i ) {
		os << ( i == 0 ? "" : ", " ) << a;
	});
	os << "]";
	return os;
}  
	
#endif
	
	
