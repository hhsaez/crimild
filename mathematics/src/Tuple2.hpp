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
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_MATHEMATICS_TUPLE_2_
#define CRIMILD_MATHEMATICS_TUPLE_2_

#include "Concepts.hpp"

#include <cassert>
#include <cmath>

namespace crimild {

    /**
     * @brief Base class for two-dimensional classes like vectors and points
     *
     * @see
     * [Tuples](mathematics/README.md)
     */
    template< template< ArithmeticType > class Derived, ArithmeticType T >
    class Tuple2 {
    protected:
        /**
         * @name Constructors
         *
         * @details
         * Constructors are made protected to enphasize that Tuple2 is not a class
         * that can be instantiated directly, but it should be used through derived classes.
         *
         * @remarks
         * None of the constructors check if the values are floating-point "not a number"
         * values (NaNs), both for performance and logic reasons. This choice allows Tuple2 to
         * potentially represent NaN values, which is left to the user to decide if that situation
         * is valid or not. For example, while in most cases a NaN value could be considered a bug,
         * there is a valid case where they are used to split line segments in shaders.
         */
        ///@{

        /**
         * \brief Default constructor
         */
        constexpr Tuple2( void ) noexcept = default;

        /**
         * \brief Constructs a Tuple2 from a single value
         *
         * All members will be set to the same value.
         */
        constexpr explicit Tuple2( T value ) noexcept
            : Tuple2( value, value )
        {
            // do nothing
        }

        /**
         * \brief Explicit constructor
         */
        constexpr Tuple2( T x, T y ) noexcept
            : x( x ),
              y( y )
        {
            // do nothing
        }

        /**
         * \brief Copy constructor
         */
        template< ArithmeticType U >
        constexpr Tuple2( const Tuple2< Derived, U > &other ) noexcept
            : Tuple2( other.x, other.y )
        {
            // do nothing
        }

        ///@}

    public:
        /**
         * \brief Default destructor
         */
        ~Tuple2( void ) = default;

        ///@{
        /**
         * Tuple2 stores its coordinates as public variables. One could argue that public
         * variables are not very "object-oriented", but the truth is that encapsulation
         * is not really appropriated here and, in fact, ends up adding more code with no
         * real benefit.
         *
         * As a side note, the use of curly braces ensures they are default initialized
         * (numeric types will be initialized to zero by default).
         *
         * @warning
         * Derived classes might require to bring these variable into scope by doing
         * @code
         *  using Tuple2< SomeDerivedClass, T >::x;
         *  using Tuple2< SomeDerivedClass, T >::y;
         * @endcode
         */
        T x = {};
        T y = {};
        ///@}

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator=( const Tuple2< Derived, U > &other ) noexcept
        {
            x = other.x;
            y = other.y;
            return static_cast< Derived< T > & >( *this );
        }

        ///@{
        /**
         * @brief Index-based accessors for tuple components
         *
         * @details
         * Some rutines do find it more useful to access tuple components inside a loop. The non-const
         * overload returns a reference, allowing to set the values of each component while indexing.
         */
        [[nodiscard]] inline constexpr T operator[]( size_t index ) const noexcept
        {
            assert( index >= 0 && index <= 1 && "Invalid index" );
            return index == 0 ? x : y;
        }

        [[nodiscard]] inline constexpr T &operator[]( size_t index ) noexcept
        {
            assert( index >= 0 && index <= 1 && "Invalid index" );
            return index == 0 ? x : y;
        }
        ///@}

        ///@{
        /**
         * @brief Comparison operators
         *
         * In order to prevent ambiguities, these overloads use Tuple2 as param. It is still
         * not possible to compare objects of different Derived types, though, so comparing
         * Vector2 and Point2 instances is not allowed.
         *
         * @remarks
         * These operators perform strict equality for tuple components. In some scenarios,
         * you might want to use isEqual() instead for comparing floating-point values.
         *
         * @see isEqual
         */
        template< ArithmeticType U >
        inline constexpr bool operator==( const Tuple2< Derived, U > &other ) const noexcept
        {
            return x == other.x && y == other.y;
        }

        template< ArithmeticType U >
        inline constexpr bool operator!=( const Tuple2< Derived, U > &other ) const noexcept
        {
            return !( *this == other );
        }
        ///@}

        /**
         * @name Arithmetic operations
         *
         * Arithmetic operations that operate on the values of a tuple. While the operations themselves
         * are trivial and well known, the code is a bit complex, since most of them work with the Derived
         * class (instead of Tuple2, which might seem more intuitive).
         *
         * For starters, most operations are templated based on another type `U`, supporting operating over
         * two elements of the same Derived template class, but with potentially different storage types for
         * their components (i.e. an addition between a Vector2<float> and a Vector2<int>).
         *
         * The return type of each operation is also worth noting. First, they return `Derived`, so any
         * operation involving, for example, Vector2, will also return a Vector2. This prevents the case of
         * operations returning the wrong implementation (if we use Tuple2 instead, it might be "legal" to
         * return a different type, like Point2, instead which won't make sense). Also, the storage type
         * of the returned type is determined based on both input types (`T` and `U`). Then, we use
         * standard C++ type promotion rules: adding a floating-point based Vector2 to an integer based one
         * will result in a floating-point Vector2 as expected.
         *
         * @remarks
         * It is still possible for derived classes to implement additional arithmetic operations for special
         * cases. For example, while Tuple2 does not supported, it is possible to add a Vector2 to a Point2
         * to obtain a new Point2 (the inverse is not allowed, though). This operation is defined in Point2.
         */
        ///@{
        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator+( const Derived< U > &other ) const noexcept
        {
            return Derived< decltype( T {} + U {} ) > {
                x + other.x,
                y + other.y,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator+=( const Derived< U > &other ) noexcept
        {
            x += other.x;
            y += other.y;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator-( const Derived< U > &other ) const noexcept
        {
            return Derived< decltype( T {} - U {} ) > {
                x - other.x,
                y - other.y,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator-=( const Derived< U > &other ) noexcept
        {
            x -= other.x;
            y -= other.y;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator*( const U &scalar ) const noexcept
        {
            return Derived< decltype( T {} * U {} ) > {
                x * scalar,
                y * scalar,
            };
        }

        /**
         * @brief Multiplication overload for premultiplying with a scalar
         */
        template< ArithmeticType U >
        [[nodiscard]] friend inline constexpr auto operator*( const U &scalar, const Derived< T > &u ) noexcept
        {
            return Derived< decltype( T {} * U {} ) > {
                u.x * scalar,
                u.y * scalar,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator*=( const U &s ) noexcept
        {
            x *= s;
            y *= s;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator*( const Derived< U > &other ) const noexcept
        {
            return Derived< decltype( T {} * U {} ) > {
                x * other.x,
                y * other.y,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator*=( const Derived< U > &other ) noexcept
        {
            x *= other.x;
            y *= other.y;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator/( const U &scalar ) const noexcept
        {
            return Derived< decltype( T {} / U {} ) > {
                x / scalar,
                y / scalar,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator/=( const U &s ) noexcept
        {
            x /= s;
            y /= s;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator/( const Derived< U > &other ) const noexcept
        {
            return Derived< decltype( T {} / U {} ) > {
                x / other.x,
                y / other.y,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator/=( const Derived< U > &other ) noexcept
        {
            x /= other.x;
            y /= other.y;
            return static_cast< Derived< T > & >( *this );
        }

        inline constexpr Derived< T > operator-( void ) const noexcept
        {
            return Derived< T >( -x, -y );
        }
        ///@}
    };

}

#endif
