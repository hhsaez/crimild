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

#ifndef CRIMILD_MATHEMATICS_VECTOR_3_CONSTANTS_
#define CRIMILD_MATHEMATICS_VECTOR_3_CONSTANTS_

#include "Mathematics/Vector3.hpp"

#include <limits>

namespace crimild {

    template< typename T >
    struct Vector3Impl< T >::Constants {
        static constexpr auto ZERO = Vector3Impl< T > { 0, 0, 0 };
        static constexpr auto ONE = Vector3Impl< T > { 1, 1, 1 };
        static constexpr auto POSITIVE_INFINITY = Vector3Impl< T > {
            std::numeric_limits< T >::infinity(),
            std::numeric_limits< T >::infinity(),
            std::numeric_limits< T >::infinity(),
        };
        static constexpr auto NEGATIVE_INFINITY = Vector3Impl< T > {
            -std::numeric_limits< T >::infinity(),
            -std::numeric_limits< T >::infinity(),
            -std::numeric_limits< T >::infinity(),
        };
        static constexpr auto UNIT_X = Vector3Impl< T > { 1, 0, 0 };
        static constexpr auto UNIT_Y = Vector3Impl< T > { 0, 1, 0 };
        static constexpr auto UNIT_Z = Vector3Impl< T > { 0, 0, 1 };
        static constexpr auto RIGHT = Vector3Impl< T > { 1, 0, 0 };
        static constexpr auto UP = Vector3Impl< T > { 0, 1, 0 };
        static constexpr auto FORWARD = Vector3Impl< T > { 0, 0, -1 };
    };

}

#endif