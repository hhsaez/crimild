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

#ifndef CRIMILD_MATHEMATICS_TUPLE_COMPONENTS_
#define CRIMILD_MATHEMATICS_TUPLE_COMPONENTS_

#include "Mathematics/Normal3.hpp"
#include "Mathematics/Point2.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Tuple2.hpp"
#include "Mathematics/Tuple3.hpp"
#include "Mathematics/Tuple4.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"

namespace crimild {

    namespace traits {

        template< template< typename > class TupleImpl >
        [[nodiscard]] constexpr Size tupleComponents( void ) noexcept { return 0; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Tuple2Impl >( void ) noexcept { return 2; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Tuple3Impl >( void ) noexcept { return 3; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Tuple4Impl >( void ) noexcept { return 4; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Point2Impl >( void ) noexcept { return 2; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Point3Impl >( void ) noexcept { return 3; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Vector2Impl >( void ) noexcept { return 2; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Vector3Impl >( void ) noexcept { return 3; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Vector4Impl >( void ) noexcept { return 4; }

        template<>
        [[nodiscard]] constexpr Size tupleComponents< Normal3Impl >( void ) noexcept { return 3; }

    }

}

#endif