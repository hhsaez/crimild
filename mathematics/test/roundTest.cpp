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

#include "round.hpp"

#include "isEqual.hpp"

#include "gtest/gtest.h"

TEST( round, default )
{
    static_assert( crimild::isEqual( 2.0, crimild::round( 2.3 ) ) );
    static_assert( crimild::isEqual( 4.0, crimild::round( 3.8 ) ) );
    static_assert( crimild::isEqual( 6.0, crimild::round( 5.5 ) ) );
    static_assert( crimild::isEqual( -2.0, crimild::round( -2.3 ) ) );
    static_assert( crimild::isEqual( -4.0, crimild::round( -3.8 ) ) );
    static_assert( crimild::isEqual( -6.0, crimild::round( -5.5 ) ) );

    EXPECT_TRUE( true );
}

TEST( round, customThreshold )
{
    static_assert( crimild::isEqual( 2.0, crimild::round( 2.3, 0.3 ) ) );
    static_assert( crimild::isEqual( 4.0, crimild::round( 3.8, 0.3 ) ) );
    static_assert( crimild::isEqual( 6.0, crimild::round( 5.5, 0.3 ) ) );
    static_assert( crimild::isEqual( 5.0, crimild::round( 5.2, 0.3 ) ) );
    static_assert( crimild::isEqual( -3.0, crimild::round( -2.31, 0.3 ) ) );
    static_assert( crimild::isEqual( -4.0, crimild::round( -3.8, 0.3 ) ) );
    static_assert( crimild::isEqual( -6.0, crimild::round( -5.5, 0.3 ) ) );
    static_assert( crimild::isEqual( -5.0, crimild::round( -5.2, 0.3 ) ) );

    EXPECT_TRUE( true );
}
