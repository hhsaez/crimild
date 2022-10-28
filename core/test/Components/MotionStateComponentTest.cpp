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

#include "Components/MotionStateComponent.hpp"

#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Mathematics/Point_equality.hpp"
#include "Mathematics/Vector_equality.hpp"

#include <gtest/gtest.h>

using namespace crimild;

TEST( MotionState, rtti )
{
    ASSERT_STREQ( "crimild::MotionState", MotionState::__CLASS_NAME );

    auto motion = crimild::alloc< MotionState >();
    ASSERT_STREQ( "crimild::MotionState", motion->getClassName() );
}

TEST( MotionState, default_values )
{
    auto motion = crimild::alloc< MotionState >();

    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
}

TEST( MotionState, coding )
{
    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();

    {
        auto motion = crimild::alloc< MotionState >();
        motion->velocity = Vector3 { 1, 2, 3 };
        motion->position = Point3 { 4, 5, 6 };
        motion->steering = Vector3 { 7, 8, 9 };

        encoder->encode( motion );
    }

    {
        auto bytes = encoder->getBytes();
        decoder->fromBytes( bytes );

        auto motion = decoder->getObjectAt< MotionState >( 0 );
        ASSERT_NE( nullptr, motion );
        EXPECT_EQ( ( Vector3 { 1, 2, 3 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 4, 5, 6 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 7, 8, 9 } ), motion->steering );
    }
}
