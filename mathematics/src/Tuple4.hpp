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

#ifndef CRIMILD_MATHEMATICS_TUPLE_4_
#define CRIMILD_MATHEMATICS_TUPLE_4_

#include "concepts.hpp"

#include <cassert>
#include <cmath>

namespace crimild {

    /**
     * @brief Base class for four-dimensional classes like vectors and points
     *
     * @details
     * This template class provides common utilities for all four dimensional
     * classes. Makes use of the "curiously recurring template pattern" (CRTP)
     * to know the type of a Derived class (i.e. Vector4) and, therefore, the
     * exact type that should be returned from some of its generic functions below
     * (see the various operators overloads, for example). This helps reducing
     * the amount of repeated code in derived classes, while also ensuring
     * those operations cannot be mixed (if we use Tuple4 instead of the
     * type of a derived class, we could end up mixing Vector4 and Point4 in
     * some operations that are not supposed to mix them).
     *
     * The use of CRTP makes Tuple4 a kind of an abstract class, although no actual
     * abstract virtual functions are used for performance reasons.
     */
    template< template< ArithmeticType > class Derived, ArithmeticType T >
    class Tuple4 {
    protected:
        /**
         * @name Constructors
         *
         * @details
         * Constructors are made protected to enphasize that Tuple4 is not a class
         * that can be instantiated directly, but it should be used through derived classes.
         *
         * @remarks
         * None of the constructors check if the values are floating-point "not a number"
         * values (NaNs), both for performance and logic reasons. This choice allows Tuple4 to
         * potentially represent NaN values, which is left to the user to decide if that situation
         * is valid or not. For example, while in most cases a NaN value could be considered a bug,
         * there is a valid case where they are used to split line segments in shaders.
         */
        ///@{

        /**
         * \brief Default constructor
         */
        constexpr Tuple4( void ) noexcept = default;

        /**
         * \brief Constructs a Tuple2 from a single value
         *
         * All members will be set to the same value.
         */
        constexpr explicit Tuple4( T value ) noexcept
            : Tuple4( value, value, value, value )
        {
            // do nothing
        }

        /**
         * \brief Explicit constructor
         */
        constexpr Tuple4( T x, T y, T z, T w ) noexcept
            : x( x ),
              y( y ),
              z( z ),
              w( w )
        {
            // do nothing
        }

        /**
         * \brief Copy constructor
         */
        template< ArithmeticType U >
        constexpr Tuple4( const Tuple4< Derived, U > &other ) noexcept
            : Tuple4( other.x, other.y, other.z, other.w )
        {
            // do nothing
        }

        ///@}

    public:
        /**
         * \brief Default destructor
         */
        ~Tuple4( void ) = default;

        ///@{
        /**
         * Tuple4 stores its coordinates as public variables. One could argue that public
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
         *  using Tuple4< SomeDerivedClass, T >::x;
         *  using Tuple4< SomeDerivedClass, T >::y;
         *  using Tuple4< SomeDerivedClass, T >::z;
         *  using Tuple4< SomeDerivedClass, T >::w;
         * @endcode
         */
        T x = {};
        T y = {};
        T z = {};
        T w = {};
        ///@}

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator=( const Tuple4< Derived, U > &other ) noexcept
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
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
            assert( index >= 0 && index <= 3 && "Invalid index" );
            switch ( index ) {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
                default:
                    return w;
            }
        }

        [[nodiscard]] inline constexpr T &operator[]( size_t index ) noexcept
        {
            assert( index >= 0 && index <= 3 && "Invalid index" );
            switch ( index ) {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
                default:
                    return w;
            }
        }
        ///@}

        ///@{
        /**
         * @brief Comparison operators
         *
         * In order to prevent ambiguities, these overloads use Tuple4 as param. It is still
         * not possible to compare objects of different Derived types, though, so comparing
         * Vector4 and Point4 instances is not allowed.
         *
         * @remarks
         * These operators perform strict equality for tuple components. In some scenarios,
         * you might want to use isEqual() instead for comparing floating-point values.
         *
         * @see isEqual
         */
        template< ArithmeticType U >
        inline constexpr bool operator==( const Tuple4< Derived, U > &other ) const noexcept
        {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        template< ArithmeticType U >
        inline constexpr bool operator!=( const Tuple4< Derived, U > &other ) const noexcept
        {
            return !( *this == other );
        }
        ///@}

        /**
         * @name Arithmetic operations
         *
         * Arithmetic operations that operate on the values of a tuple. While the operations themselves
         * are trivial and well known, the code is a bit complex, since most of them work with the Derived
         * class (instead of Tuple4, which might seem more intuitive).
         *
         * For starters, most operations are templated based on another type `U`, supporting operating over
         * two elements of the same Derived template class, but with potentially different storage types for
         * their components (i.e. an addition between a Vector4<float> and a Vector4<int>).
         *
         * The return type of each operation is also worth noting. First, they return `Derived`, so any
         * operation involving, for example, Vector4, will also return a Vector4. This prevents the case of
         * operations returning the wrong implementation (if we use Tuple4 instead, it might be "legal" to
         * return a different type, like Point4, instead which won't make sense). Also, the storage type
         * of the returned type is determined based on both input types (`T` and `U`). Then, we use
         * standard C++ type promotion rules: adding a floating-point based Vector4 to an integer based one
         * will result in a floating-point Vector4 as expected.
         *
         * @remarks
         * It is still possible for derived classes to implement additional arithmetic operations for special
         * cases. For example, while Tuple4 does not supported, it is possible to add a Vector4 to a Point4
         * to obtain a new Point4 (the inverse is not allowed, though). This operation is defined in Point4.
         */
        ///@{
        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator+( const Derived< U > &other ) const noexcept
        {
            return Derived< decltype( T {} + U {} ) > {
                x + other.x,
                y + other.y,
                z + other.z,
                w + other.w,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator+=( const Derived< U > &other ) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator-( const Derived< U > &other ) const noexcept
        {
            return Derived< decltype( T {} - U {} ) > {
                x - other.x,
                y - other.y,
                z - other.z,
                w - other.w,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator-=( const Derived< U > &other ) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator*( const U &scalar ) const noexcept
        {
            return Derived< decltype( T {} * U {} ) > {
                x * scalar,
                y * scalar,
                z * scalar,
                w * scalar,
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
                u.z * scalar,
                u.w * scalar,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator*=( const U &s ) noexcept
        {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator*( const Derived< U > &other ) const noexcept
        {
            return Derived< decltype( T {} * U {} ) > {
                x * other.x,
                y * other.y,
                z * other.z,
                w * other.w,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator*=( const Derived< U > &other ) noexcept
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator/( const U &scalar ) const noexcept
        {
            return Derived< decltype( T {} / U {} ) > {
                x / scalar,
                y / scalar,
                z / scalar,
                w / scalar,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator/=( const U &s ) noexcept
        {
            x /= s;
            y /= s;
            z /= s;
            w /= s;
            return static_cast< Derived< T > & >( *this );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr Derived< T > operator/( const Derived< U > &other ) const noexcept
        {
            return Derived< decltype( T {} / U {} ) > {
                x / other.x,
                y / other.y,
                z / other.z,
                w / other.w,
            };
        }

        template< ArithmeticType U >
        inline constexpr Derived< T > &operator/=( const Derived< U > &other ) noexcept
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return static_cast< Derived< T > & >( *this );
        }

        inline constexpr Derived< T > operator-( void ) const noexcept
        {
            return Derived< T >( -x, -y, -z, -w );
        }
        ///@}
    };

}

#endif
