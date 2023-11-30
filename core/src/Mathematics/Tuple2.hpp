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

#include "Foundation/Types.hpp"
#include "Mathematics/Concepts.hpp"

#include <cassert>
#include <cmath>

namespace crimild {

    /**
     * @brief Base class for two-dimensional classes like vectors and points
     *
     * @details
     * This template class provides common utilities for all two dimensional
     * classes. Makes use of the "curiously recurring template pattern" (CRTP)
     * to know the type of a Derived class (i.e. Vector2) and, therefore, the
     * exact type that should be returned from some of its generic functions below
     * (see the various operators overloads, for example). This helps reducing
     * the amount of repeated code in derived classes, while also ensuring
     * those operations cannot be mixed (if we use Tuple2 instead of the
     * type of a derived class, we could end up mixing Vector2 and Point2 in
     * some operations that are not supposed to mix them).
     *
     * The use of CRTP makes Tuple2 a kind of an abstract class, although no actual
     * abstract virtual functions are used for performance reasons.
     */
    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    class Tuple2 {
    protected:
        /**
         * \brief Default constructor
         */
        constexpr Tuple2( void ) noexcept = default;

        /**
         * \brief Constructs a tuple from a single value
         *
         * All members will be set to the same value.
         */
        constexpr explicit Tuple2( T value ) noexcept
            : Tuple2( value, value )
        {
        }

        /**
         * \brief Explicit constructor
         */
        constexpr Tuple2( T x, T y ) noexcept
            : x( x ),
              y( y )
        {
        }

        /**
         * \brief Copy constructor
         */
        template< concepts::Arithmetic U >
        constexpr Tuple2( const Tuple2< Derived, U > &other ) noexcept
            : Tuple2( other.x, other.y )
        {
        }

    public:
        /**
         * \brief Default destructor
         */
        ~Tuple2( void ) = default;

        /**
         * Tuple2 stores its coordinates as public variables
         *
         * Also, the use of curly braces ensures they are default initialized (numeric
         * types will be initialized to zero by default).
         */
        ///@{
        T x = {};
        T y = {};
        ///@}

        template< concepts::Arithmetic U >
        inline constexpr Derived< T > &operator=( const Tuple2< Derived, U > &other ) noexcept
        {
            x = other.x;
            y = other.y;
            return static_cast< Derived< T > & >( *this );
        }

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

        template< concepts::Arithmetic U >
        inline constexpr Derived< T > &operator+=( const Derived< U > &other ) noexcept
        {
            x += other.x;
            y += other.y;
            return static_cast< Derived< T > & >( *this );
        }

        template< concepts::Arithmetic U >
        inline constexpr Derived< T > &operator-=( const Derived< U > &other ) noexcept
        {
            x -= other.x;
            y -= other.y;
            return static_cast< Derived< T > & >( *this );
        }

        template< concepts::Arithmetic U >
        inline constexpr Derived< T > &operator*=( const U &s ) noexcept
        {
            x *= s;
            y *= s;
            return static_cast< Derived< T > & >( *this );
        }

        template< concepts::Arithmetic U >
        inline constexpr Derived< T > &operator*=( const Derived< U > &other ) noexcept
        {
            x *= other.x;
            y *= other.y;
            return static_cast< Derived< T > & >( *this );
        }

        template< concepts::Arithmetic U >
        inline constexpr Derived< T > &operator/=( const U &s ) noexcept
        {
            x /= s;
            y /= s;
            return static_cast< Derived< T > & >( *this );
        }

        template< concepts::Arithmetic U >
        inline constexpr Derived< T > &operator/=( const Derived< U > &other ) noexcept
        {
            x /= other.x;
            y /= other.y;
            return static_cast< Derived< T > & >( *this );
        }
    };

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator==( const Tuple2< Derived, T > &u, const Tuple2< Derived, U > &v ) noexcept
    {
        return u.x == v.x && u.y == v.y;
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator!=( const Tuple2< Derived, T > &u, const Tuple2< Derived, U > &v ) noexcept
    {
        return !( u == v );
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator+( const Tuple2< Derived, T > &u, const Tuple2< Derived, U > &v ) noexcept
    {
        return Derived< decltype( T {} + U {} ) > {
            u.x + v.x,
            u.y + v.y,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator-( const Tuple2< Derived, T > &u, const Tuple2< Derived, U > &v ) noexcept
    {
        return Derived< decltype( T {} - U {} ) > {
            u.x - v.x,
            u.y - v.y,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator*( const Tuple2< Derived, T > &u, const U &s ) noexcept
    {
        return Derived< decltype( T {} * U {} ) > {
            u.x * s,
            u.y * s,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator*( const U &s, const Tuple2< Derived, T > &u ) noexcept
    {
        return Derived< decltype( T {} * U {} ) > {
            u.x * s,
            u.y * s,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator*( const Tuple2< Derived, T > &u, const Tuple2< Derived, U > &v ) noexcept
    {
        return Derived< decltype( T {} * U {} ) > {
            u.x * v.x,
            u.y * v.y,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator/( const Tuple2< Derived, T > &u, const U &s ) noexcept
    {
        return u * ( 1 / s );
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator/( const Tuple2< Derived, T > &u, const Tuple2< Derived, U > &v ) noexcept
    {
        return Derived< decltype( T {} / U {} ) > {
            u.x / v.x,
            u.y / v.y,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto operator-( const Tuple2< Derived, T > &u ) noexcept
    {
        return Derived< T > {
            -u.x,
            -u.y,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto xx( const Tuple2< Derived, T > &u ) noexcept
    {
        return Derived< T > {
            u.x,
            u.x,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto xy( const Tuple2< Derived, T > &u ) noexcept
    {
        return Derived< T > {
            u.x,
            u.y,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto yx( const Tuple2< Derived, T > &u ) noexcept
    {
        return Derived< T > {
            u.y,
            u.x,
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto yy( const Tuple2< Derived, T > &u ) noexcept
    {
        return Derived< T > {
            u.y,
            u.y,
        };
    }

    template< typename T >
    struct [[deprecated]] Tuple2Impl {
        T x;
        T y;

        [[nodiscard]] inline constexpr T operator[]( Size index ) const noexcept
        {
            switch ( index ) {
                case 0:
                    return x;
                case 1:
                    return y;
                default:
                    return NAN;
            }
        }

        [[nodiscard]] inline constexpr T &operator[]( Size index ) noexcept
        {
            switch ( index ) {
                case 0:
                    return x;
                case 1:
                default:
                    return y;
            }
        }
    };

}

#endif
