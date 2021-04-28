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

#ifndef CRIMILD_MATHEMATICS_NUMBERS_
#define CRIMILD_MATHEMATICS_NUMBERS_

#include "Foundation/Types.hpp"
#include "Mathematics/Traits.hpp"

#include <limits>

namespace crimild {

    namespace numbers {

        inline constexpr auto E = Real( 2.7182818284590452353602874713526624977572470936999595749669676277 );
        inline constexpr auto LOG2_E = Real( 1.4426950408889634073599246810018921374266459541529859341354494069 );
        inline constexpr auto LOG10_E = Real( 0.4342944819032518276511289189166050822943970058036665661144537831 );

        inline constexpr auto PI = Real( 3.1415926535897932384626433832795028841971693993751058209749445923 );
        inline constexpr auto PI_DIV_2 = Real( 1.5707963267948966192313216916397514420985846996875529104874722961 );
        inline constexpr auto PI_DIV_3 = Real( 1.0471975511965977461542144610931676280657231331250352736583148641 );
        inline constexpr auto PI_DIV_4 = Real( 0.7853981633974483096156608458198757210492923498437764552437361480 );
        inline constexpr auto TWO_PI = Real( 6.2831853071795864769252867665590057683943387987502116419498891846 );
        inline constexpr auto INV_PI = Real( 0.3183098861837906715377675267450287240689192914809128974953346881 );
        inline constexpr auto INV_SQRT_PI = Real( 0.5641895835477562869480794515607725858440506293289988568440857217 );

        inline constexpr auto LN_2 = Real( 0.6931471805599453094172321214581765680755001343602552541206800094 );
        inline constexpr auto LN_10 = Real( 2.3025850929940456840179914546843642076011014886287729760333279009 );

        inline constexpr auto SQRT_2 = Real( 1.4142135623730950488016887242096980785696718753769480731766797379 );
        inline constexpr auto INV_SQRT_2 = Real( 0.7071067811865475244008443621048490392848359376884740365883398689 );
        inline constexpr auto SQRT_2_DIV_2 = Real( 0.7071067811865475244008443621048490392848359376884740365883398689 );
        inline constexpr auto SQRT_3 = Real( 1.7320508075688772935274463415058723669428052538103806280558069794 );
        inline constexpr auto INV_SQRT_3 = Real( 0.5773502691896257645091487805019574556476017512701268760186023264 );

        inline constexpr auto EGAMMA = Real( 0.5772156649015328606065120900824024310421593359399235988057672348 );
        inline constexpr auto PHI = Real( 1.6180339887498948482045868343656381177203091798057628621354486227 );

        inline constexpr auto EPSILON = [] {
            if constexpr ( traits::isHighPrecision< Real >() ) {
                return 1e-10;
            } else {
                return 1e-4;
            }
        }();

        inline constexpr auto COS_0 = Real( 1 );
        inline constexpr auto COS_30 = Real( 0.8660254037844386467637231707529361834714026269051903140279034897 );
        inline constexpr auto COS_45 = Real( 0.7071067811865475244008443621048490392848359376884740365883398689 );
        inline constexpr auto COS_90 = Real( 0 );
        inline constexpr auto COS_180 = Real( -1 );

        inline constexpr auto POSITIVE_INFINITY = std::numeric_limits< Real >::max();
        inline constexpr auto NEGATIVE_INFINITY = std::numeric_limits< Real >::min();
        inline constexpr auto INFINITY = POSITIVE_INFINITY;

    }

}

#endif
