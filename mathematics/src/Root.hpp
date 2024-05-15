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

#ifndef CRIMILD_MATHEMATICS_ROOT_
#define CRIMILD_MATHEMATICS_ROOT_

#include "Types.hpp"

namespace crimild {

    [[nodiscard]] static constexpr int findRoots( real_t a, real_t b, real_t c, real_t &t0, real_t &t1 ) noexcept
    {
        const auto discriminant = b * b - 4 * a * c;

        if ( discriminant < 0 ) {
            return 0; // no real roots
        } else {
            real_t s = -b / 2 * a;

            if ( discriminant == 0 ) {
                t0 = t1 = s;
                return 1; // one real root
            } else {
                const auto sqrtDiscriminant = sqrt( discriminant ) / ( 2 * a );
                t0 = s - sqrtDiscriminant;
                t1 = s + sqrtDiscriminant;
                return 2; // two real roots
            }
        }
    }

    /**
                \brief Find the roots for a polynomial
         */
    class [[deprecated]] Root {
    public:
        /**
                        \brief Determine if the polynomial has real roots
                 */
        template< typename T >
        static bool hasRealRoots( T a, T b, T c )
        {
            T discriminant = b * b - 4 * a * c;

            if ( discriminant < 0 ) {
                return false; // no real roots
            } else {
                return true; // real roots exist
            }
        }

        /**
                        \brief Calculate roots for a quadratic polynomial
                        \returns Number of roots found (0, 1 or 2)

                        If there is no real roots for this polynomial, the
                        calculate() function returns 0
                 */
        template< typename T >
        static int compute( T a, T b, T c, T &t0, T &t1 )
        {
            T discriminant = b * b - 4 * a * c;

            if ( discriminant < 0 ) {
                return 0; // no real roots
            } else {
                T s = -b / 2 * a;

                if ( discriminant == 0 ) {
                    t0 = t1 = s;
                    return 1; // one real root
                } else {
                    T sqrtDiscriminant = static_cast< T >( std::sqrt( discriminant ) / ( 2 * a ) );
                    t0 = s - sqrtDiscriminant;
                    t1 = s + sqrtDiscriminant;
                    return 2; // two real roots
                }
            }
        }
    };

}

#endif
