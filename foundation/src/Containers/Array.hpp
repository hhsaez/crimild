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

#ifndef CRIMILD_FOUNDATION_CONTAINERS_ARRAY_
#define CRIMILD_FOUNDATION_CONTAINERS_ARRAY_

#include "Common/Types.hpp"
#include "Policies/ThreadingPolicy.hpp"

#include <functional>
#include <iostream>
#include <algorithm>

namespace crimild {

    /**
       \brief A resizable array implementation

       \todo Implement index bound checking policy
       \todo Implement parallel policy
    */
    template<
        typename T,
        class ThreadingPolicy = policies::SingleThreaded >
    class Array : public ThreadingPolicy {
    private:
        using LockImpl = typename ThreadingPolicy::Lock;

    public:
        Array( void ) noexcept
        {
            resize_unsafe( 1 );
        }

        explicit Array( Size size, T *data = nullptr ) noexcept
        {
            resize_unsafe( size );

            _size = size;

            if ( data != nullptr ) {
                for ( Size i = 0; i < _size; i++ ) {
                    _elems[ i ] = data[ i ];
                }
            }
        }

        Array( std::initializer_list< T > l ) noexcept
            : Array( l.size() )
        {
            _size = 0;
            for ( auto e : l ) {
                _elems[ _size++ ] = e;
            }
        }

        Array( const Array &other ) noexcept
            : Array( other._size )
        {
            for ( Size i = 0; i < _size; i++ ) {
                _elems[ i ] = other._elems[ i ];
            }
        }

        Array( Array &&other ) noexcept
            : _elems( std::move( other._elems ) ),
              _size( other._size ),
              _capacity( other._capacity )
        {
            other._size = 0;
            other._capacity = 0;
        }

        virtual ~Array( void ) noexcept
        {
            resize_unsafe( 0 );
            _size = 0;
        }

        Array &operator=( const Array &other ) noexcept
        {
            LockImpl lock( this );

            resize_unsafe( other._capacity );
            _size = other._size;
            for ( Size i = 0; i < _size; i++ ) {
                _elems[ i ] = other._elems[ i ];
            }

            return *this;
        }

        Array &operator=( Array &&other ) noexcept
        {
            LockImpl lock( this );

            _elems = std::move( other._elems );
            _size = other._size;
            _capacity = other._capacity;

            other._size = 0;
            other._capacity = 0;

            return *this;
        }

        /**
           \brief Compare two arrays up to getSize() elements
        */
        Bool operator==( const Array &other ) const noexcept
        {
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

        inline Bool operator!=( const Array &other ) const noexcept
        {
            return !( *this == other );
        }

        inline bool empty( void ) const noexcept
        {
            LockImpl lock( this );
            return size_unsafe() == 0;
        }

        inline Size size( void ) const noexcept
        {
            LockImpl lock( this );
            return size_unsafe();
        }

        inline void clear( void ) noexcept
        {
            LockImpl lock( this );
            resize_unsafe( 0 );
            _size = 0;
        }

        inline T &operator[]( Size index ) noexcept
        {
            LockImpl lock( this );
            return _elems[ index ];
        }

        inline const T &operator[]( Size index ) const noexcept
        {
            LockImpl lock( this );
            return _elems[ index ];
        }

        inline T *getData( void ) noexcept
        {
            LockImpl lock( this );
            return &_elems[ 0 ];
        }

        inline const T *getData( void ) const noexcept
        {
            LockImpl lock( this );
            return &_elems[ 0 ];
        }

        inline T &first( void ) noexcept
        {
            LockImpl lock( this );
            return _elems[ 0 ];
        }

        inline const T &first( void ) const noexcept
        {
            LockImpl lock( this );
            return _elems[ 0 ];
        }

        inline T &last( void ) noexcept
        {
            LockImpl lock( this );
            return _elems[ _size - 1 ];
        }

        inline const T &last( void ) const noexcept
        {
            LockImpl lock( this );
            return _elems[ _size - 1 ];
        }

        inline bool indexOf( T const &elem, size_t &index ) const noexcept
        {
            LockImpl lock( this );

            index = 0;
            while ( index < _size ) {
                if ( _elems[ index ] == elem ) {
                    return true;
                }
                ++index;
            }
            return false;
        }

        void add( T const &elem ) noexcept
        {
            LockImpl lock( this );
            if ( _size == _capacity ) {
                resize_unsafe( 2 * _capacity );
            }

            _elems[ _size++ ] = elem;
        }

        void addAt( T const &elem, size_t index ) noexcept
        {
            LockImpl lock( this );
            if ( _size == _capacity ) {
                resize_unsafe( 2 * _capacity );
            }

            // Shift elements after index, starting at the back
            for ( size_t i = _size; i > index; --i ) {
                _elems[ i ] = _elems[ i - 1 ];
            }
            // add new element
            _elems[ index ] = elem;

            // Increment size
            ++_size;
        }

        Array &remove( const T &elem ) noexcept
        {
            LockImpl lock( this );

            Size i = 0;

            while ( i < _size && _elems[ i ] != elem ) {
                i++;
            }

            if ( i < _size ) {
                removeAt_unsafe( i );
            }

            return *this;
        }

        Array &removeAt( Size index ) noexcept
        {
            LockImpl lock( this );
            removeAt_unsafe( index );
            return *this;
        }

        Array &swap( Size i, Size j ) noexcept
        {
            LockImpl lock( this );
            swap_unsafe( i, j );
            return *this;
        }

        Array &resize( Size capacity ) noexcept
        {
            LockImpl lock( this );
            resize_unsafe( capacity );
            _size = capacity;
            return *this;
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

        template< typename Callback >
        Array &sort( Callback callback ) noexcept
        {
            LockImpl lock( this );
            if ( _size == 0 ) {
                return *this;
            }

            std::sort( &_elems[ 0 ], &_elems[ _size ], callback );
            return *this;
        }

        template< typename Callback >
        Array &each( Callback callback ) noexcept
        {
            LockImpl lock( this );

            // implement a policy to traverse the array by creating a copy if needed
            for ( Size i = 0; i < _size; i++ ) {
                callback( _elems[ i ] );
            }

            return *this;
        }

        template< typename Callback >
        const Array &each( Callback callback ) const noexcept
        {
            LockImpl lock( this );

            // implement a policy to traverse the array by creating a copy if needed
            for ( Size i = 0; i < _size; i++ ) {
                callback( _elems[ i ] );
            }

            return *this;
        }

        template< typename Selector >
        Array filter( Selector selector ) const noexcept
        {
            Array ret;
            for ( Size i = 0; i < _size; ++i ) {
                if ( selector( _elems[ i ] ) ) {
                    ret.add( _elems[ i ] );
                }
            }
            return ret;
        }

        template< typename Mapper >
        auto map( Mapper mapper ) const noexcept
        {
            Array< decltype( mapper( _elems[ 0 ] ) ) > ret( _size );
            for ( auto i = 0l; i < _size; i++ ) {
                ret[ i ] = mapper( _elems[ i ] );
            }
            return ret;
        }

        template< typename Fn >
        Array &fill( Fn fn ) noexcept
        {
            for ( auto i = 0l; i < _size; i++ ) {
                _elems[ i ] = fn( i );
            }
            return *this;
        }

        Array reversed( void ) const noexcept
        {
            auto ret = Array( _size );
            for ( auto i = 0l; i < _size; i++ ) {
                ret[ _size - i - 1 ] = _elems[ i ];
            }
            return ret;
        }

    private:
        inline Size size_unsafe( void ) const noexcept
        {
            return _size;
        }

        void resize_unsafe( Size capacity ) noexcept
        {
            capacity = std::max( size_t( 1 ), size_t( capacity ) );
            auto elems = std::unique_ptr< T[] >( new T[ capacity ] );
            auto count = std::min( capacity, _capacity );
            for ( Size i = 0; i < count; i++ ) {
                elems[ i ] = _elems[ i ];
            }
            _capacity = capacity;
            _elems = std::move( elems );
        }

        void swap_unsafe( Size i, Size j ) noexcept
        {
            auto t = _elems[ i ];
            _elems[ i ] = _elems[ j ];
            _elems[ j ] = t;
        }

        void removeAt_unsafe( Size index ) noexcept
        {
            for ( Size i = index; i < _size - 1; i++ ) {
                _elems[ i ] = _elems[ i + 1 ];
            }
            _elems[ _size - 1 ] = T(); // avoid loitering
            --_size;

            if ( _size > 0 && _size == _capacity / 4 ) {
                // resize the array if needed
                resize_unsafe( _capacity / 2 );
            }
        }

    private:
        std::unique_ptr< T[] > _elems;
        Size _size = 0;
        Size _capacity = 0;

    public:
        friend std::ostream &operator<<( std::ostream &os, const Array &array ) noexcept
        {
            os << "[";
            array.each(
                [ &os,
                  i = 0 ]( const T &a ) mutable {
                    os << ( i++ == 0 ? "" : ", " ) << a;
                }
            );
            os << "]";
            return os;
        }
    };

    template< typename T >
    using ThreadSafeArray = Array< T, policies::ObjectLevelLockable >;

    using ByteArray = Array< Byte >;

}

#endif
