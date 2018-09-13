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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_QUEUE_
#define CRIMILD_FOUNDATION_CONTAINERS_QUEUE_

#include "Foundation/Types.hpp"
#include "Foundation/Policies/ThreadingPolicy.hpp"
#include "Mathematics/Numeric.hpp"

#include <functional>
#include <iostream>
#include <deque>
#include <algorithm>

namespace crimild {

	namespace containers {

		/**
		   \brief A resizable queue implementation
		   
		   \todo Implement index bound checking policy
		   \todo Implement parallel policy
		*/
		template<
		    typename T,
		    class ThreadingPolicy = policies::SingleThreaded
		>
		class Queue : public ThreadingPolicy {
		private:
			using LockImpl = typename ThreadingPolicy::Lock;
			
		public:
			using TraverseCallback = std::function< void( T &, crimild::Size ) >;
			using ConstTraverseCallback = std::function< void( const T &, crimild::Size ) >;
			
		public:
			Queue( void )
			{

			}
			
			Queue( const Queue &other )
				: _impl( other._impl )
			{

			}
			
			virtual ~Queue( void )
			{

			}
            
            Queue &operator=( const Queue &other )
            {
                LockImpl lock( this );
				_impl = other._impl;
				return *this;
            }

			/**
			   \brief Compare two stacks up to getSize() elements
			 */
			bool operator==( const Queue &other ) const
			{
                LockImpl lock( this );
                return _impl == other._impl;
			}
			
			inline bool empty( void )
			{
				LockImpl lock( this );
				return _impl.empty();
			}
			
			inline crimild::Size size( void )
			{
				LockImpl lock( this );
				return _impl.size();
			}
			
            inline void clear( void )
            {
                LockImpl lock( this );
				return _impl.clear();
            }
            
			void push( T const &elem )
			{
				LockImpl lock( this );
				_impl.push_back( elem );
			}
			
			T pop( void )
			{
				LockImpl lock( this );

				T front = _impl.front();
				_impl.pop_front();
				return front;
			}
            
            T &front( void )
            {
                return _impl.front();
            }

			const T &top( void ) const
			{
				return _impl.front();
			}
			
			crimild::Bool contains( const T &e ) const
			{
				LockImpl lock( this );
				return std::find( _impl.begin(), _impl.end(), e ) != _impl.end();
			}

			void remove( const T &elem )
			{
				LockImpl lock( this );
				_impl.erase( std::remove( _impl.begin(), _impl.end(), elem ), _impl.end() );
			}
			
			void each( TraverseCallback const &callback )
			{
				LockImpl lock( this );
				crimild::Size i = 0;
				for ( auto &e : _impl ) {
					callback( e, i++ );
				}
			}

			void each( ConstTraverseCallback const &callback ) const
			{
				LockImpl lock( this );
				crimild::Size i = 0;
				for ( auto &e : _impl ) {
					callback( e, i++ );
				}
			}

		private:
			std::deque< T > _impl;
		};
		
	}

}

template<
    typename T,
    class TP
>
std::ostream& operator<<( std::ostream& os, const crimild::containers::Queue< T, TP > &s )  
{  
	os << "[";
	s.each( [ &os ]( const T &a, crimild::Size i ) {
		os << ( i == 0 ? "" : ", " ) << a;
	});
	os << "]";
	return os;
}  
	
#endif
	
	
