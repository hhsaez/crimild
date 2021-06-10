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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_SET_
#define CRIMILD_FOUNDATION_CONTAINERS_SET_

#include "Array.hpp"
#include "Foundation/Policies/ThreadingPolicy.hpp"
#include "Foundation/Types.hpp"

#include <functional>
#include <iostream>
#include <unordered_set>

namespace crimild {

    /**
       \brief A set implementation

       \todo Implement index bound checking policy
       \todo Implement parallel policy
    */
    template<
        typename ValueType,
        class ThreadingPolicy = policies::SingleThreaded,
        class SetImpl = std::unordered_set< ValueType > >
    class Set : public ThreadingPolicy {
    private:
        using LockImpl = typename ThreadingPolicy::Lock;

    public:
        Set( void ) = default;

        Set( std::initializer_list< ValueType > l ) noexcept
            : _set( l )
        {
        }

        Set( const Set &other ) noexcept
            : _set( other._set )
        {
        }

        virtual ~Set( void ) noexcept
        {
            _set.clear();
        }

        Set &operator=( const Set &other ) noexcept
        {
            LockImpl lock( this );
            _set = other._set;
            return *this;
        }

        Bool operator==( const Set &other ) const noexcept
        {
            LockImpl lock( this );
            return _set == other._set;
        }

        inline Bool empty( void ) const noexcept
        {
            LockImpl lock( this );
            return _set.empty();
        }

        inline Size size( void ) const noexcept
        {
            LockImpl lock( this );
            return _set.size();
        }

        inline void clear( void ) noexcept
        {
            LockImpl lock( this );
            _set.clear();
        }

        inline bool contains( const ValueType &value ) const noexcept
        {
            LockImpl lock( this );
            return _set.count( value ) > 0;
        }

        void insert( ValueType const &value ) noexcept
        {
            LockImpl lock( this );
            _set.insert( value );
        }

        void remove( ValueType const &value ) noexcept
        {
            LockImpl lock( this );
            _set.erase( value );
        }

        template< typename Callback >
        void each( Callback callback ) noexcept
        {
            LockImpl lock( this );
            auto it = std::begin( _set );
            while ( it != std::end( _set ) ) {
                auto &elem = *it;
                // Increase iterator first to allow element removal on callback
                ++it;
                callback( elem );
            }
        }

        template< typename Callback >
        void each( Callback const &callback ) const noexcept
        {
            LockImpl lock( this );
            for ( const auto &v : _set ) {
                callback( v );
            }
        }

        inline ValueType first( void ) noexcept { return *std::begin( _set ); }
        inline const ValueType &first( void ) const noexcept { return *std::begin( _set ); }

        template< typename Fn >
        Set filter( Fn selector ) const noexcept
        {
            Set ret;
            each(
                [ & ]( const auto &elem ) {
                    if ( selector( elem ) ) {
                        ret.insert( elem );
                    }
                } );
            return ret;
        }

        template< typename Fn >
        auto map( Fn fn ) const noexcept
        {
            ValueType dummy;
            Set< decltype( fn( dummy ) ) > ret {};
            each(
                [ & ]( const auto &elem ) {
                    ret.insert( fn( elem ) );
                } );
            return ret;
        }

    private:
        SetImpl _set;

    public:
        std::ostream &operator<<( std::ostream &os ) const noexcept
        {
            os << "[";
            Bool first = true;
            each( [ &os, &first ]( const auto &v ) {
                os << ( first == 0 ? "" : ", " ) << v;
                first = false;
            } );
            os << "]";
            return os;
        }
    };

    template< typename VALUE_TYPE >
    using ThreadSafeSet = Set< VALUE_TYPE, policies::ObjectLevelLockable >;

}

#endif
