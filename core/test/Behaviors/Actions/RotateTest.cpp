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

#include "Behaviors/Actions/Rotate.hpp"

#include "Behaviors/withBehavior.hpp"
#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "Mathematics/Vector_equality.hpp"
#include "SceneGraph/Node.hpp"

#include <gtest/gtest.h>

using namespace crimild;
using namespace crimild::behaviors;

TEST( Rotate, coding )
{
    auto rotate = crimild::alloc< behaviors::actions::Rotate >( Vector3 { 0, 1, 0 }, 0.5f );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    encoder->encode( rotate );
    auto bytes = encoder->getBytes();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();
    decoder->fromBytes( bytes );

    auto decoded = decoder->getObjectAt< behaviors::actions::Rotate >( 0 );
    ASSERT_TRUE( decoded != nullptr );
    EXPECT_EQ( rotate->getAxis(), decoded->getAxis() );
    EXPECT_EQ( rotate->getAngle(), decoded->getAngle() );
}

TEST( Rotate, coding_with_rotate )
{
    auto node = behaviors::withBehavior(
        crimild::alloc< Node >(),
        behaviors::actions::rotate(
            Vector3::Constants::UNIT_Y,
            0.1f ) );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    encoder->encode( node );
    auto bytes = encoder->getBytes();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();
    decoder->fromBytes( bytes );

    auto decoded = decoder->getObjectAt< Node >( 0 );
    ASSERT_NE( nullptr, decoded );
    ASSERT_NE( nullptr, decoded->getComponent< BehaviorController >() );
    ASSERT_NE( nullptr, decoded->getComponent< BehaviorController >()->getBehaviorTree() );
    ASSERT_NE( nullptr, decoded->getComponent< BehaviorController >()->getBehaviorTree()->getRootBehavior() );

    auto rotate = static_cast< behaviors::actions::Rotate * >( decoded->getComponent< BehaviorController >()->getBehaviorTree()->getRootBehavior() );
    EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), rotate->getAxis() );
    EXPECT_EQ( 0.1f, rotate->getAngle() );
}