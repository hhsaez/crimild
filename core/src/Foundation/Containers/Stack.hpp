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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_STACK_
#define CRIMILD_FOUNDATION_CONTAINERS_STACK_

#include "Foundation/Types.hpp"
#include "Foundation/Policies/ThreadingPolicy.hpp"
#include "Mathematics/Numeric.hpp"

#include <functional>
#include <iostream>

namespace crimild {

	namespace containers {

		/**
		   \brief A resizable stack implementation
		   
		   \todo Implement index bound checking policy
		   \todo Implement parallel policy
		*/
		template<
		    typename T,
		    class ThreadingPolicy = policies::SingleThreaded
		>
		class Stack : public ThreadingPolicy {
		private:
			using LockImpl = typename ThreadingPolicy::Lock;
			
		public:
			using TraverseCallback = std::function< void( T &, crimild::Size ) >;
			using ConstTraverseCallback = std::function< void( const T &, crimild::Size ) >;
			
		public:
			Stack( void )
			{
				resize_unsafe( 2 );
			}
			
			Stack( const Stack &other )
			{
				resize_unsafe( other._capacity );
				
                for ( crimild::Size i = 0; i < _capacity; i++ ) {
                    _elems[ i ] = other._elems[ i ];
                }
                
                _size = other._size;
			}
			
			virtual ~Stack( void )
			{
				
			}
            
            Stack &operator=( const Stack &other )
            {
                LockImpl lock( this );
                
                resize_unsafe( other._capacity );
                for ( crimild::Size i = 0; i < _size; i++ ) {
                    _elems[ i ] = other._elems[ i ];
                }
                _size = other._size;
            }

			/**
			   \brief Compare two stacks up to getSize() elements
			 */
			bool operator==( const Stack &other ) const
			{
                LockImpl lock( this );
                
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
			
			inline bool empty( void )
			{
				LockImpl lock( this );
				return size_unsafe() == 0;
			}
			
			inline crimild::Size size( void )
			{
				LockImpl lock( this );
				return size_unsafe();
			}
			
			void push( T const &elem )
			{
				LockImpl lock( this );
				
				if ( _size == _capacity ) {
					resize_unsafe( 2 * _size );
				}

				_elems[ _size++ ] = elem;
			}
			
			T pop( void )
			{
				LockImpl lock( this );

				T a = _elems[ _size - 1 ];
				_elems[ _size - 1 ] = T(); // avoid loitering
				_size--;

				if ( _size > 0 && _size == _capacity / 4 ) {
					resize_unsafe( _capacity / 2 );
				}

				return a;
			}
            
            T &top( void )
            {
                return _elems[ _size - 1 ];
            }

			const T &top( void ) const
			{
				return _elems[ _size - 1 ];
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

			void remove( const T &elem )
			{
				LockImpl lock( this );

				crimild::Size i = 0;
				
				while ( i < _size && _elems[ i ] != elem ) {
					i++;
				}
				
				if ( i < _size ) {
					for ( ; i < _size - 1; i++ ) {
						_elems[ i ] = _elems[ i + 1 ];
					}
					_elems[ _size - 1 ] = T(); // avoid loitering
					--_size;
					
					if ( _size > 0 && _size == _capacity / 4 ) {
						// resize the array if needed
						resize_unsafe( _size / 2 );
					}			
				}
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
				auto elems = std::unique_ptr< T[] >( new T[ capacity ] );
                for ( crimild::Size i = 0; i < Numeric< crimild::Size >::min( capacity, _capacity ); i++ ) {
                    elems[ i ] = _elems[ i ];
                }
                _capacity = capacity;
				_elems = std::move( elems );
			}

		private:
			std::unique_ptr< T[] > _elems;
			crimild::Size _size = 0;
			crimild::Size _capacity = 0;
		};
		
	}

}

template<
    typename T,
    class TP
>
std::ostream& operator<<( std::ostream& os, const crimild::containers::Stack< T, TP > &s )  
{  
	os << "[";
	s.each( [&os]( const T &a, crimild::Size i ) {
		os << ( i == 0 ? "" : ", " ) << a;
	});
	os << "]";
	return os;
}  
	
#endif
	
	
