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

#include "Simulation/Input.hpp"

#include "Simulation/Event.hpp"

#include <gtest/gtest.h>

using namespace crimild;

// Helper functions
auto keyEvent = []( auto type, auto key ) {
    Input::getInstance()->handle(
        Event {
            .type = type,
            .keyboard {
                .key = uint32_t( key ),
            },
        }
    );
};

TEST( Input, it_handles_key_events )
{
    Input input;

    EXPECT_FALSE( input.isKeyDown( CRIMILD_INPUT_KEY_W ) );
    EXPECT_TRUE( input.isKeyUp( CRIMILD_INPUT_KEY_W ) );

    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_W );

    EXPECT_TRUE( input.isKeyDown( CRIMILD_INPUT_KEY_W ) );
    EXPECT_FALSE( input.isKeyUp( CRIMILD_INPUT_KEY_W ) );

    EXPECT_FALSE( input.isKeyDown( CRIMILD_INPUT_KEY_S ) );
    EXPECT_TRUE( input.isKeyUp( CRIMILD_INPUT_KEY_S ) );

    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_W );

    EXPECT_FALSE( input.isKeyDown( CRIMILD_INPUT_KEY_W ) );
    EXPECT_TRUE( input.isKeyUp( CRIMILD_INPUT_KEY_W ) );
}

TEST( Input, it_updates_positive_vertical_axis_when_pressing_w_key )
{
    Input input;

    EXPECT_EQ( 0, input.getAxis( Input::AXIS_VERTICAL ) );
    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_W );
    EXPECT_EQ( 1, input.getAxis( Input::AXIS_VERTICAL ) );
    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_W );
    EXPECT_EQ( 0, input.getAxis( Input::AXIS_VERTICAL ) );
}

TEST( Input, it_updates_negative_vertical_axis_when_pressing_s_key )
{
    Input input;

    EXPECT_EQ( 0, input.getAxis( Input::AXIS_VERTICAL ) );
    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_S );
    EXPECT_EQ( -1, input.getAxis( Input::AXIS_VERTICAL ) );
    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_S );
    EXPECT_EQ( 0, input.getAxis( Input::AXIS_VERTICAL ) );
}

TEST( Input, it_updates_positive_horizontal_axis_when_pressing_d_key )
{
    Input input;

    EXPECT_EQ( 0, input.getAxis( Input::AXIS_HORIZONTAL ) );
    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_D );
    EXPECT_EQ( 1, input.getAxis( Input::AXIS_HORIZONTAL ) );
    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_D );
    EXPECT_EQ( 0, input.getAxis( Input::AXIS_HORIZONTAL ) );
}

TEST( Input, it_updates_negative_horizontal_axis_when_pressing_a_key )
{
    Input input;

    EXPECT_EQ( 0, input.getAxis( Input::AXIS_HORIZONTAL ) );
    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_A );
    EXPECT_EQ( -1, input.getAxis( Input::AXIS_HORIZONTAL ) );
    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_A );
    EXPECT_EQ( 0, input.getAxis( Input::AXIS_HORIZONTAL ) );
}

TEST( Input, it_updates_positive_vertical_axis_when_pressing_up_key )
{
    Input input;

    EXPECT_EQ( 0, input.getAxis( Input::AXIS_VERTICAL ) );
    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_UP );
    EXPECT_EQ( 1, input.getAxis( Input::AXIS_VERTICAL ) );
    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_UP );
    EXPECT_EQ( 0, input.getAxis( Input::AXIS_VERTICAL ) );
}

TEST( Input, it_updates_negative_vertical_axis_when_pressing_down_key )
{
    Input input;

    EXPECT_EQ( 0, input.getAxis( Input::AXIS_VERTICAL ) );
    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_DOWN );
    EXPECT_EQ( -1, input.getAxis( Input::AXIS_VERTICAL ) );
    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_DOWN );
    EXPECT_EQ( 0, input.getAxis( Input::AXIS_VERTICAL ) );
}

TEST( Input, it_updates_positive_horizontal_axis_when_pressing_right_key )
{
    Input input;

    EXPECT_EQ( 0, input.getAxis( Input::AXIS_HORIZONTAL ) );
    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_RIGHT );
    EXPECT_EQ( 1, input.getAxis( Input::AXIS_HORIZONTAL ) );
    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_RIGHT );
    EXPECT_EQ( 0, input.getAxis( Input::AXIS_HORIZONTAL ) );
}

TEST( Input, it_updates_negative_horizontal_axis_when_pressing_left_key )
{
    Input input;

    EXPECT_EQ( 0, input.getAxis( Input::AXIS_HORIZONTAL ) );
    keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_LEFT );
    EXPECT_EQ( -1, input.getAxis( Input::AXIS_HORIZONTAL ) );
    keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_LEFT );
    EXPECT_EQ( 0, input.getAxis( Input::AXIS_HORIZONTAL ) );
}
