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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_LIST_
#define CRIMILD_FOUNDATION_CONTAINERS_LIST_

#include "Foundation/Types.hpp"
#include "Foundation/Policies/ThreadingPolicy.hpp"
#include "Mathematics/Numeric.hpp"

#include <functional>
#include <iostream>
#include <list>

namespace crimild {

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
        List( void ) = default;

        List( std::initializer_list< T > l ) noexcept
            : _list( l )
        {

        }

        List( const List &other ) noexcept
            : _list( other._list )
        {

        }

        List( List &&other ) noexcept
            : _list( std::move( other._list ) )
        {

        }

        virtual ~List( void ) noexcept
        {
            _list.clear();
        }

        List &operator=( const List &other ) noexcept
        {
            LockImpl lock( this );

            _list = other._list;

            return *this;
        }

        List &operator=( List &&other ) noexcept
        {
            LockImpl lock( this );

            _list = std::move( other._list );

            return *this;
        }

        /**
           \brief Compare two arrays up to getSize() elements
        */
        inline bool operator==( const List &other ) const noexcept
        {
            return _list == other._list;
        }

        inline bool empty( void ) const noexcept
        {
            LockImpl lock( this );
            return _list.size() == 0;
        }

        inline Size size( void ) const noexcept
        {
            LockImpl lock( this );
            return _list.size();
        }

        inline void clear( void ) noexcept
        {
            LockImpl lock( this );
            _list.clear();
        }

        inline T &first( void ) noexcept
        {
            LockImpl lock( this );

            return _list.front();
        }

        void add( T const &elem ) noexcept
        {
            LockImpl lock( this );

            _list.push_back( elem );
        }

        void remove( const T &elem ) noexcept
        {
            LockImpl lock( this );

            _list.remove( elem );
        }

        Bool contains( const T &e ) const noexcept
        {
            LockImpl lock( this );

            return std::find( std::begin( _list ), std::end( _list ), e ) != std::end( _list );
        }

        template< typename Callback >
        void each( Callback callback ) noexcept
        {
            LockImpl lock( this );

            // implement a policy to traverse the array by creating a copy if needed
            for ( auto e : _list ) {
                callback( e );
            }
        }

        template< typename Callback >
        void each( Callback callback ) const noexcept
        {
            LockImpl lock( this );

            // implement a policy to traverse the array by creating a copy if needed
            for ( auto e : _list ) {
                callback( e );
            }
        }

    private:
        std::list< T > _list;

    public:
        friend std::ostream& operator<<( std::ostream& os, const List &list ) noexcept
        {
            os << "[";
            list.each( [ &os, i = 0 ]( const T &a ) mutable {
                os << ( i++ == 0 ? "" : ", " ) << a;
            });
            os << "]";

            return os;
        }

    };

    template< typename T >
    using ThreadSafeList = List< T, policies::ObjectLevelLockable >;
}

#endif
