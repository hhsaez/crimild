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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_STACK_
#define CRIMILD_FOUNDATION_CONTAINERS_STACK_

#include "Common/Types.hpp"
#include "Policies/ThreadingPolicy.hpp"

#include <functional>
#include <iostream>

namespace crimild {

    /**
       \brief A resizable stack implementation

       \todo Implement index bound checking policy
       \todo Implement parallel policy
    */
    template<
        typename T,
        class ThreadingPolicy = policies::SingleThreaded >
    class Stack : public ThreadingPolicy {
    private:
        using LockImpl = typename ThreadingPolicy::Lock;

    public:
        Stack( void ) noexcept
        {
            resize_unsafe( 2 );
        }

        Stack( const Stack &other ) noexcept
        {
            resize_unsafe( other._capacity );

            for ( Size i = 0; i < _capacity; i++ ) {
                _elems[ i ] = other._elems[ i ];
            }

            _size = other._size;
        }

        virtual ~Stack( void ) noexcept
        {
            resize_unsafe( 2 );
            _size = 0;
        }

        Stack &operator=( const Stack &other ) noexcept
        {
            LockImpl lock( this );

            resize_unsafe( other._capacity );
            for ( Size i = 0; i < _size; i++ ) {
                _elems[ i ] = other._elems[ i ];
            }
            _size = other._size;
        }

        /**
           \brief Compare two stacks up to getSize() elements
        */
        bool operator==( const Stack &other ) const noexcept
        {
            LockImpl lock( this );

            if ( _size != other._size ) {
                return false;
            }

            for ( Size i = 0; i < _size; i++ ) {
                if ( _elems[ i ] != other._elems[ i ] ) {
                    return false;
                }
            }

            return true;
        }

        inline bool empty( void ) noexcept
        {
            LockImpl lock( this );
            return size_unsafe() == 0;
        }

        inline Size size( void ) noexcept
        {
            LockImpl lock( this );
            return size_unsafe();
        }

        inline void clear( void ) noexcept
        {
            LockImpl lock( this );
            resize_unsafe( 2 );
            _size = 0;
        }

        void push( T const &elem ) noexcept
        {
            LockImpl lock( this );

            if ( _size == _capacity ) {
                resize_unsafe( 2 * _size );
            }

            _elems[ _size++ ] = elem;
        }

        T pop( void ) noexcept
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

        T &top( void ) noexcept
        {
            return _elems[ _size - 1 ];
        }

        const T &top( void ) const noexcept
        {
            return _elems[ _size - 1 ];
        }

        Bool contains( const T &e ) const noexcept
        {
            LockImpl lock( this );
            for ( Size i = 0; i < _size; i++ ) {
                if ( _elems[ i ] == e ) {
                    return true;
                }
            }
            return false;
        }

        void remove( const T &elem ) noexcept
        {
            LockImpl lock( this );

            Size i = 0;

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

        template< typename Callback >
        void each( Callback callback ) noexcept
        {
            LockImpl lock( this );

            // implement a policy to traverse the array by creating a copy if needed
            for ( Size i = 0; i < _size; i++ ) {
                callback( _elems[ i ] );
            }
        }

        template< typename Callback >
        void each( Callback callback ) const noexcept
        {
            LockImpl lock( this );

            // implement a policy to traverse the array by creating a copy if needed
            for ( Size i = 0; i < _size; i++ ) {
                callback( _elems[ i ] );
            }
        }

    private:
        inline Size size_unsafe( void ) const noexcept
        {
            return _size;
        }

        void resize_unsafe( Size capacity ) noexcept
        {
            auto elems = std::unique_ptr< T[] >( new T[ capacity ] );
            for ( Size i = 0; i < std::min( capacity, _capacity ); i++ ) {
                elems[ i ] = _elems[ i ];
            }
            _capacity = capacity;
            _elems = std::move( elems );
        }

    private:
        std::unique_ptr< T[] > _elems;
        Size _size = 0;
        Size _capacity = 0;

    public:
        friend std::ostream &operator<<( std::ostream &os, const Stack &s ) noexcept
        {
            os << "[";
            s.each( [ &os ]( const T &a, Size i ) {
                os << ( i == 0 ? "" : ", " ) << a;
            } );
            os << "]";
            return os;
        }
    };

}

#endif
