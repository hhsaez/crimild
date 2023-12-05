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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_MAP_
#define CRIMILD_FOUNDATION_CONTAINERS_MAP_

#include "Common/Types.hpp"
#include "Containers/Array.hpp"
#include "Policies/ThreadingPolicy.hpp"

#include <functional>
#include <iostream>
#include <unordered_map>

namespace crimild {

    /**
       \brief A map implementation

       \todo Implement index bound checking policy
       \todo Implement parallel policy
    */
    template<
        typename KeyType,
        typename ValueType,
        class ThreadingPolicy = policies::SingleThreaded,
        class MapImpl = std::unordered_map< KeyType, ValueType > >
    class Map : public ThreadingPolicy {
    private:
        using LockImpl = typename ThreadingPolicy::Lock;

    public:
        Map( void ) = default;

        Map( std::initializer_list< std::pair< const KeyType, ValueType > > l ) noexcept
            : _map( l )
        {
        }

        Map( const Map &other ) noexcept
            : _map( other._map )
        {
        }

        virtual ~Map( void ) noexcept
        {
            _map.clear();
        }

        Map &operator=( const Map &other ) noexcept
        {
            LockImpl lock( this );
            _map = other._map;
            return *this;
        }

        /**
           \brief Compare two arrays up to getSize() elements
        */
        inline bool operator==( const Map &other ) const noexcept
        {
            LockImpl lock( this );
            return _map == other._map;
        }

        inline bool empty( void ) const noexcept
        {
            LockImpl lock( this );
            return _map.empty();
        }

        inline Size size( void ) const noexcept
        {
            LockImpl lock( this );
            return _map.size();
        }

        inline void clear( void ) noexcept
        {
            LockImpl lock( this );
            _map.clear();
        }

        inline bool contains( const KeyType &key ) const noexcept
        {
            LockImpl lock( this );
            return _map.count( key ) > 0;
        }

        inline ValueType &operator[]( const KeyType &key ) noexcept
        {
            LockImpl lock( this );
            return _map[ key ];
        }

        inline const ValueType &operator[]( const KeyType &key ) const noexcept
        {
            LockImpl lock( this );
            return _map.at( key );
        }

        void insert( const KeyType &key, ValueType const &value ) noexcept
        {
            LockImpl lock( this );
            _map[ key ] = value;
        }

        void remove( const KeyType &key ) noexcept
        {
            LockImpl lock( this );
            _map.erase( key );
        }

        Array< KeyType > keys( void ) const noexcept
        {
            Array< KeyType > ret( size() );
            Size i = 0;
            for ( const auto &it : _map ) {
                ret[ i++ ] = it.first;
            }
            return ret;
        }

        Array< ValueType > values( void ) const noexcept
        {
            Array< ValueType > ret( size() );
            Size i = 0;
            for ( const auto &it : _map ) {
                ret[ i++ ] = it.second;
            }
            return ret;
        }

        template< typename Callback >
        void each( Callback callback ) noexcept
        {
            LockImpl lock( this );
            for ( auto &it : _map ) {
                callback( it.first, it.second );
            }
        }

        template< typename Callback >
        void each( Callback callback ) const noexcept
        {
            LockImpl lock( this );
            for ( const auto &it : _map ) {
                callback( it.first, it.second );
            }
        }

        template< typename Callback >
        void eachKey( Callback callback ) noexcept
        {
            LockImpl lock( this );
            for ( auto &it : _map ) {
                callback( it.first );
            }
        }

        template< typename Callback >
        void eachKey( Callback callback ) const noexcept
        {
            LockImpl lock( this );
            for ( const auto &it : _map ) {
                callback( it.first );
            }
        }

        template< typename Callback >
        void eachValue( Callback callback ) noexcept
        {
            LockImpl lock( this );
            for ( auto &it : _map ) {
                callback( it.second );
            }
        }

        template< typename Callback >
        void eachValue( Callback callback ) const noexcept
        {
            LockImpl lock( this );
            for ( const auto &it : _map ) {
                callback( it.second );
            }
        }

    private:
        MapImpl _map;

    public:
        friend std::ostream &operator<<( std::ostream &os, const Map &map ) noexcept
        {
            os << "{";
            Bool first = true;
            map.each( [ &os, &first ]( const auto &k, const auto &v ) {
                os << ( first == 0 ? "" : ", " ) << "[" << k << ": " << v << "]";
                first = false;
            } );
            os << "}";
            return os;
        }
    };

    template< typename KEY_TYPE, typename VALUE_TYPE >
    using ThreadSafeMap = Map< KEY_TYPE, VALUE_TYPE, policies::ObjectLevelLockable >;

}

#endif
