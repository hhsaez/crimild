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

#ifndef CRIMILD_MATHEMATICS_POINT_3_
#define CRIMILD_MATHEMATICS_POINT_3_

#include "Mathematics/Tuple3.hpp"

#include <limits>

namespace crimild {

    namespace impl {

        template< typename T >
        struct Point3 : public Tuple3< T > {
            struct Constants;
        };

        template< typename T >
        struct Point3< T >::Constants {
            static constexpr auto ZERO = Point3< T > { 0, 0, 0 };
            static constexpr auto ONE = Point3< T > { 1, 1, 1 };
            static constexpr auto POSITIVE_INFINITY = Point3< T > { std::numeric_limits< T >::max(), std::numeric_limits< T >::max(), std::numeric_limits< T >::max() };
            static constexpr auto NEGATIVE_INFINITY = Point3< T > { std::numeric_limits< T >::min(), std::numeric_limits< T >::min(), std::numeric_limits< T >::min() };
        };

    }

    using Point3 = impl::Point3< Real >;
    using Point3f = impl::Point3< Real32 >;
    using Point3d = impl::Point3< Real64 >;
    using Point3i = impl::Point3< Int32 >;
    using Point3ui = impl::Point3< UInt32 >;

}

#endif
