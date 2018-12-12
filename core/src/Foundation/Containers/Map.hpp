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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_MAP_
#define CRIMILD_FOUNDATION_CONTAINERS_MAP_

#include "Array.hpp"

#include "Foundation/Types.hpp"
#include "Foundation/Policies/ThreadingPolicy.hpp"

#include <functional>
#include <iostream>
#include <unordered_map>

namespace crimild {

	namespace containers {

		/**
		   \brief A map implementation
		   
		   \todo Implement index bound checking policy
		   \todo Implement parallel policy
		*/
		template<
		    typename KeyType,
		    typename ValueType,
		    class ThreadingPolicy = policies::SingleThreaded,
		    class MapImpl = std::unordered_map< KeyType, ValueType >
		>
		class Map : public ThreadingPolicy {
		private:
			using LockImpl = typename ThreadingPolicy::Lock;
			
		public:
			using TraverseCallback = std::function< void( const KeyType &, ValueType & ) >;
			using ConstTraverseCallback = std::function< void( const KeyType &, const ValueType & ) >;
			using KeyTraverseCallback = std::function< void( KeyType const & ) >;
			using ConstKeyTraverseCallback = std::function< void( const KeyType & ) >;
			using ValueTraverseCallback = std::function< void( ValueType & ) >;
			using ConstValueTraverseCallback = std::function< void( const ValueType & ) >;
			
		public:
			Map( void )
			{
				
			}
			
			Map( std::initializer_list< std::pair< const KeyType, ValueType >> l )
				: _map( l )
			{

			}

			Map( const Map &other )
				: _map( other._map )
			{

			}
			
			virtual ~Map( void )
			{
                _map.clear();
			}
            
            Map &operator=( const Map &other )
            {
                LockImpl lock( this );
				_map = other._map;
				return *this;
            }

			/**
			   \brief Compare two arrays up to getSize() elements
			 */
			bool operator==( const Map &other ) const
			{
				LockImpl lock( this );
				return _map == other._map;
			}
			
			inline bool empty( void ) const
			{
				LockImpl lock( this );
				return _map.empty();
			}
			
			inline crimild::Size size( void ) const
			{
				LockImpl lock( this );
				return _map.size();
			}

			inline void clear( void )
			{
				LockImpl lock( this );
				_map.clear();
			}

			inline bool contains( const KeyType &key ) const
			{
				LockImpl lock( this );
				return _map.count( key ) > 0;
			}
			
			inline ValueType &operator[]( const KeyType &key )
			{
				LockImpl lock( this );
                return _map[ key ];
			}
			
            inline const ValueType &operator[]( const KeyType &key ) const
            {
                LockImpl lock( this );
                return _map.at( key );
            }

			void insert( const KeyType &key, ValueType const &value )
			{
				LockImpl lock( this );
				_map[ key ] = value;
			}
			
			void remove( const KeyType &key )
			{
				LockImpl lock( this );
				_map.erase( key );
			}

			Array< KeyType > keys( void ) const
			{
				Array< KeyType > ret( size() );
				crimild::Size i = 0;
				for ( const auto &it : _map ) {
					ret[ i++ ] = it.first;
				}
				return ret;
			}

			Array< ValueType > values( void ) const
			{
				Array< ValueType > ret( size() );
				crimild::Size i = 0;
				for ( const auto &it : _map ) {
					ret[ i++ ] = it.second;
				}
				return ret;
			}
			
			void each( TraverseCallback const &callback )
			{
				LockImpl lock( this );
				for ( auto &it : _map ) {
					callback( it.first, it.second );
				}
			}

			void each( ConstTraverseCallback const &callback ) const
			{
				LockImpl lock( this );
				for ( const auto &it : _map ) {
					callback( it.first, it.second );
				}
			}

			void eachKey( KeyTraverseCallback const &callback )
			{
				LockImpl lock( this );
				for ( auto &it : _map ) {
					callback( it.first );
				}
			}

			void eachKey( ConstKeyTraverseCallback const &callback ) const
			{
				LockImpl lock( this );
				for ( const auto &it : _map ) {
					callback( it.first );
				}
			}

			void eachValue( ValueTraverseCallback const &callback )
			{
				LockImpl lock( this );
				for ( auto &it : _map ) {
					callback( it.second );
				}
			}

			void eachValue( ValueTraverseCallback const &callback ) const
			{
				LockImpl lock( this );
				for ( const auto &it : _map ) {
					callback( it.second );
				}
			}

		private:
			MapImpl _map;
		};

		template< typename KEY_TYPE, typename VALUE_TYPE >
		using ThreadSafeMap = Map< KEY_TYPE, VALUE_TYPE, policies::ObjectLevelLockable >;
		
	}

}

template<
    typename KT,
    typename VT,
    class TP
>
std::ostream& operator<<( std::ostream& os, const crimild::containers::Map< KT, VT, TP > &map )  
{  
	os << "{";
	crimild::Bool first = true;
	map.each( [ &os, &first ]( const KT &k, const VT &v ) {
		os << ( first == 0 ? "" : ", " ) << "[" << k << ": " << v << "]";
		first = false;
	});
	os << "}";
	return os;
}  
	
#endif
	
	
