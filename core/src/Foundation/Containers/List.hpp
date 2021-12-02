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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_LIST_
#define CRIMILD_FOUNDATION_CONTAINERS_LIST_

#include "Foundation/Types.hpp"
#include "Foundation/Policies/ThreadingPolicy.hpp"
#include "Mathematics/Numeric.hpp"

#include <functional>
#include <iostream>
#include <list>

namespace crimild {

	namespace containers {

		/**
		   \brief A list implementation
		   
		   \todo Implement index bound checking policy
		   \todo Implement parallel policy
		   \todo Avoid using std::list
		*/
		template<
		    typename T,
		    class ThreadingPolicy = policies::SingleThreaded
		>
		class List : public ThreadingPolicy {
		private:
			using LockImpl = typename ThreadingPolicy::Lock;
			
		public:
			using BasicTraverseCallback = std::function< void( T & ) >;
			using ConstBasicTraverseCallback = std::function< void( const T & ) >;
			using TraverseCallback = std::function< void( T &, crimild::Size ) >;
			using ConstTraverseCallback = std::function< void( const T &, crimild::Size ) >;
			
		public:
			List( void )
			{

			}
			
			List( std::initializer_list< T > l )
				: _list( l )
			{

			}

			List( const List &other )
				: _list( other._list )
			{

			}

			List( List &&other )
				: _list( std::move( other._list ) )
			{

			}

			virtual ~List( void )
			{
                _list.clear();
			}
            
            List &operator=( const List &other )
            {
                LockImpl lock( this );

				_list = other._list;
                
                return *this;
            }

			List &operator=( List &&other )
			{
				LockImpl lock( this );

				_list = std::move( other._list );
				
				return *this;
			}

			/**
			   \brief Compare two arrays up to getSize() elements
			 */
			bool operator==( const List &other ) const
			{
				return _list == other._list;
			}
			
			inline bool empty( void ) const
			{
				LockImpl lock( this );
				return _list.size() == 0;
			}
			
			inline crimild::Size size( void ) const
			{
				LockImpl lock( this );
				return _list.size();
			}
            
            inline void clear( void )
            {
                LockImpl lock( this );
				_list.clear();
            }

			T &first( void )
			{
				LockImpl lock( this );

				return _list.front();
			}
			
			void add( T const &elem )
			{
				LockImpl lock( this );

				_list.push_back( elem );
			}
			
			void remove( const T &elem )
			{
				LockImpl lock( this );

				_list.remove( elem );
			}
			
			crimild::Bool contains( const T &e ) const
			{
				LockImpl lock( this );

				return std::find( std::begin( _list ), std::end( _list ), e ) != std::end( _list );
			}

			void each( TraverseCallback const &callback )
			{
				LockImpl lock( this );

				// implement a policy to traverse the array by creating a copy if needed
				crimild::Size i = 0;
				for ( auto e : _list ) {
					callback( e, i++ );
				}
			}

			void each( ConstTraverseCallback const &callback ) const
			{
				LockImpl lock( this );

				// implement a policy to traverse the array by creating a copy if needed
				crimild::Size i = 0;
				for ( auto e : _list ) {
					callback( e, i++ );
				}
			}

			void each( BasicTraverseCallback const &callback )
			{
				LockImpl lock( this );

				// implement a policy to traverse the array by creating a copy if needed
				for ( auto e : _list ) {
					callback( e );
				}
			}

			void each( ConstBasicTraverseCallback const &callback ) const
			{
				LockImpl lock( this );

				// implement a policy to traverse the array by creating a copy if needed
				for ( auto e : _list ) {
					callback( e );
				}
			}

		private:
			std::list< T > _list;
		};

		template< typename T >
		using ThreadSafeList = List< T, policies::ObjectLevelLockable >;
	}

}

template<
    typename T,
    class TP
>
std::ostream& operator<<( std::ostream& os, const crimild::containers::List< T, TP > &list )  
{  
	os << "[";
	list.each( [ &os ]( const T &a, crimild::Size i ) {
		os << ( i == 0 ? "" : ", " ) << a;
	});
	os << "]";
	
	return os;
}  
	
#endif
	
	
