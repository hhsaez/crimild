/*
 * Copyright (c) 2013-2018, Hernan Saez
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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_SET_
#define CRIMILD_FOUNDATION_CONTAINERS_SET_

#include "Array.hpp"

#include "Foundation/Types.hpp"
#include "Foundation/Policies/ThreadingPolicy.hpp"

#include <functional>
#include <iostream>
#include <unordered_set>

namespace crimild {

	namespace containers {

		/**
		   \brief A set implementation
		   
		   \todo Implement index bound checking policy
		   \todo Implement parallel policy
		*/
		template<
		    typename ValueType,
		    class ThreadingPolicy = policies::SingleThreaded,
		    class SetImpl = std::unordered_set< ValueType >
		>
		class Set : public ThreadingPolicy {
		private:
			using LockImpl = typename ThreadingPolicy::Lock;
			
		public:
			using TraverseCallback = std::function< void( ValueType const & ) >;
			using ConstTraverseCallback = std::function< void( const ValueType & ) >;
			
		public:
			Set( void )
			{
				
			}
			
			Set( std::initializer_list< ValueType > l )
				: _set( l )
			{

			}

			Set( const Set &other )
				: _set( other._set )
			{

			}
			
			virtual ~Set( void )
			{
                _set.clear();
			}
            
            Set &operator=( const Set &other )
            {
                LockImpl lock( this );
				_set = other._set;
				return *this;
            }

			crimild::Bool operator==( const Set &other ) const
			{
				LockImpl lock( this );
				return _set == other._set;
			}
			
			inline crimild::Bool empty( void ) const
			{
				LockImpl lock( this );
				return _set.empty();
			}
			
			inline crimild::Size size( void ) const
			{
				LockImpl lock( this );
				return _set.size();
			}

			inline void clear( void )
			{
				LockImpl lock( this );
				_set.clear();
			}

			inline bool contains( const ValueType &value ) const
			{
				LockImpl lock( this );
				return _set.count( value ) > 0;
			}
			
			void insert( ValueType const &value )
			{
				LockImpl lock( this );
				_set.insert( value );
			}
			
			void remove( ValueType const &value )
			{
				LockImpl lock( this );
				_set.erase( value );
			}

			void each( TraverseCallback const &callback )
			{
				LockImpl lock( this );
				for ( auto &v : _set ) {
					callback( v );
				}
			}

			void each( ConstTraverseCallback const &callback ) const
			{
				LockImpl lock( this );
				for ( const auto &v : _set ) {
					callback( v );
				}
			}

		private:
			SetImpl _set;
		};

		template< typename VALUE_TYPE >
		using ThreadSafeSet = Set< VALUE_TYPE, policies::ObjectLevelLockable >;
		
	}

}

template<
    typename VT,
    class TP
>
std::ostream& operator<<( std::ostream& os, const crimild::containers::Set< VT, TP > &set )  
{  
	os << "[";
	crimild::Bool first = true;
	set.each( [ &os, &first ]( const VT &v ) {
		os << ( first == 0 ? "" : ", " ) << v;
		first = false;
	});
	os << "]";
	return os;
}  
	
#endif
	
	
