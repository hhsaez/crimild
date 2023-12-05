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

#include "Containers/Set.hpp"

#include "Common/Types.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( Set, filter )
{
    auto s = Set< crimild::Int32 > {
        1,
        4,
        9,
        13,
        38,
    };

    auto f = s.filter(
        [ & ]( auto &e ) {
            // return even numbers
            return e % 2 == 0;
        }
    );

    auto expected = Set< crimild::Int32 > {
        4,
        38,
    };

    EXPECT_EQ( expected, f );
}

TEST( Set, map )
{
    auto s = Set< crimild::Int32 > {
        1,
        4,
        9,
        13,
    };

    auto m = s.map(
        [ & ]( auto &e ) {
            return 2 * e;
        }
    );

    auto expected = Set< crimild::Int32 > {
        2,
        8,
        18,
        26,
    };

    EXPECT_EQ( expected, m );
}

TEST( Set, filterMap )
{
    auto s = Set< crimild::Int32 > {
        1,
        4,
        9,
        13,
        28,
    };

    auto res = s
                   .filter(
                       [ & ]( auto &e ) {
                           return e % 2 == 0;
                       }
                   )
                   .map(
                       [ & ]( auto &e ) {
                           return 2 * e;
                       }
                   );

    auto expected = Set< crimild::Int32 > {
        8,
        56,
    };

    EXPECT_EQ( expected, res );
}
