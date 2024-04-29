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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_PRIORITY_QUEUE_
#define CRIMILD_FOUNDATION_CONTAINERS_PRIORITY_QUEUE_

#include "Common/Types.hpp"
#include "Policies/ThreadingPolicy.hpp"

#include <functional>
#include <iostream>

namespace crimild {

    /**
       \brief A priority queue

       This implementation is based on the one appearing in the
       book Algorithms 4th Edition

       \todo Implement index bound checking policy
       \todo Implement parallel policy
    */
    template<
        typename T,
        class ThreadingPolicy = policies::SingleThreaded >
    class PriorityQueue : public ThreadingPolicy {
    private:
        using LockImpl = typename ThreadingPolicy::Lock;

        using Comparator = std::function< Bool( const T &a, const T &b ) >;

    public:
        PriorityQueue( void ) noexcept
            : PriorityQueue( 1 )
        {
        }

        explicit PriorityQueue( Size capacity ) noexcept
        {
            resize_unsafe( 1 + capacity );

            _comparator = []( const T &a, const T &b ) {
                return a > b;
            }; // greater
        }

        explicit PriorityQueue( Size capacity, Comparator comparator ) noexcept
            : PriorityQueue( capacity )
        {
            if ( comparator != nullptr ) {
                _comparator = comparator;
            }
        }

        explicit PriorityQueue( Comparator comparator ) noexcept
            : PriorityQueue( 1, comparator )
        {
        }

        PriorityQueue( std::initializer_list< T > keys ) noexcept
            : PriorityQueue( keys.size() )
        {
            Size i = 0;
            for ( auto k : keys ) {
                _elems[ i + 1 ] = k;
            }
            for ( Size k = _size / 2; k >= 1; k-- ) {
                sink( k );
            }
        }

        PriorityQueue( const PriorityQueue &other ) noexcept
        {
            resize_unsafe( other._capacity );
            _size = other._size;
            _comparator = other._comparator;

            // invoke operator= on all elements (required for smart pointers)
            for ( Size i = 0; i <= _size; i++ ) {
                _elems[ i ] = other._elems[ i ];
            }
        }

        virtual ~PriorityQueue( void ) noexcept
        {
            resize_unsafe( 2 );
            _size = 0;
        }

        PriorityQueue &operator=( const PriorityQueue &other ) noexcept
        {
            LockImpl lock( this );

            resize_unsafe( other._capacity );
            _size = other._size;
            _comparator = other._comparator;

            // invoke operator= on all elements (required for smart pointers)
            for ( Size i = 0; i <= _size; i++ ) {
                _elems[ i ] = other._elems[ i ];
            }

            return *this;
        }

        /**
           \brief Compare two priority queues up to size() elements
        */
        Bool operator==( const PriorityQueue &other ) const noexcept
        {
            LockImpl lock( this );
            if ( _size != other._size ) {
                return false;
            }

            for ( Size i = 0; i <= _size; i++ ) {
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

        inline T &front( void ) noexcept
        {
            LockImpl lock( this );

            // TODO: index bound check
            return _elems[ 1 ];
        }

        inline const T &front( void ) const noexcept
        {
            LockImpl lock( this );

            // TODO: index bound check
            return _elems[ 1 ];
        }

        void enqueue( T const &elem ) noexcept
        {
            LockImpl lock( this );

            if ( _size == _capacity - 1 ) {
                resize_unsafe( 2 * _capacity );
            }

            _elems[ ++_size ] = elem;
            swim( _size );
        }

        T dequeue( void ) noexcept
        {
            LockImpl lock( this );

            T x = _elems[ 1 ];

            swap_unsafe( 1, _size-- );
            sink( 1 );

            _elems[ _size + 1 ] = T(); // avoid loitering

            if ( _size > 0 && ( _size == ( _capacity - 1 ) / 4 ) ) {
                resize_unsafe( _capacity / 2 );
            }

            return x;
        }

        void swap( Size i, Size j ) noexcept
        {
            LockImpl lock( this );

            // todo check boundaries
            swap_unsafe( i, j );
        }

        void resize( Size capacity ) noexcept
        {
            LockImpl lock( this );

            resize_unsafe( capacity );
        }

        void clear( void ) noexcept
        {
            LockImpl lock( this );

            resize_unsafe( 2 );
            _size = 0;
        }

        template< typename Callback >
        void each( Callback callback ) noexcept
        {
            LockImpl lock( this );

            // implement a policy to traverse the array by creating a copy if needed
            for ( Size i = 0; i < _size; i++ ) {
                callback( _elems[ i + 1 ], i );
            }
        }

        template< typename Callback >
        void each( Callback callback ) const noexcept
        {
            LockImpl lock( this );

            // implement a policy to traverse the array by creating a copy if needed
            for ( Size i = 0; i < _size; i++ ) {
                callback( _elems[ i + 1 ], i );
            }
        }

    private:
        inline Size size_unsafe( void ) const noexcept
        {
            return _size;
        }

        void swap_unsafe( Size i, Size j ) noexcept
        {
            auto t = _elems[ i ];
            _elems[ i ] = _elems[ j ];
            _elems[ j ] = t;
        }

        void resize_unsafe( Size capacity ) noexcept
        {
            auto elems = std::unique_ptr< T[] >( new T[ capacity ] );
            for ( Size i = 0; i < std::min( _capacity, capacity ); i++ ) {
                elems[ i ] = _elems[ i ];
            }
            _elems = std::move( elems );
            _capacity = capacity;
        }

        void swim( Size k ) noexcept
        {
            while ( k > 1 && greater( k / 2, k ) ) {
                swap_unsafe( k, k / 2 );
                k = k / 2;
            }
        }

        void sink( Size k ) noexcept
        {
            while ( 2 * k <= _size ) {
                auto j = 2 * k;
                if ( j < _size && greater( j, j + 1 ) ) {
                    j++;
                }
                if ( !greater( k, j ) ) {
                    break;
                }
                swap_unsafe( k, j );
                k = j;
            }
        }

        bool greater( Size i, Size j ) noexcept
        {
            if ( _comparator == nullptr ) {
                return _elems[ i ] > _elems[ j ];
            }
            return _comparator( _elems[ i ], _elems[ j ] );
        }

    private:
        std::unique_ptr< T[] > _elems;
        Size _size = 0;
        Size _capacity = 0;
        Comparator _comparator;

    public:
        friend std::ostream &operator<<( std::ostream &os, const PriorityQueue &pq ) noexcept
        {
            os << "[";
            pq.each( [ &os ]( const T &a, Size i ) {
                os << ( i == 0 ? "" : ", " ) << a;
            } );
            os << "]";
            return os;
        }
    };

}

#endif
