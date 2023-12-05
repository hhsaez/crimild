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

#include "Behaviors/Actions/MotionReset.hpp"

#include "Behaviors/withBehavior.hpp"
#include "Components/MotionStateComponent.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "SceneGraph/Node.hpp"

#include <gtest/gtest.h>

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

TEST( MotionReset, it_attaches_motion_state_component_on_init_if_not_present )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    ASSERT_EQ( nullptr, node->getComponent< MotionState >() );

    controller->execute( crimild::alloc< MotionReset >() );

    ASSERT_NE( nullptr, node->getComponent< MotionState >() );
}

TEST( MotionReset, it_does_not_modify_existing_state_on_init_if_already_present )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    auto motion = node->attachComponent< MotionState >();
    motion->velocity = Vector3 { 1, 2, 3 };
    motion->position = Point3f { 4, 5, 6 };
    motion->steering = Vector3 { 7, 8, 9 };

    controller->execute( crimild::alloc< MotionReset >() );

    ASSERT_NE( nullptr, node->getComponent< MotionState >() );
    EXPECT_EQ( ( Vector3 { 1, 2, 3 } ), motion->velocity );
    EXPECT_EQ( ( Point3f { 4, 5, 6 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 7, 8, 9 } ), motion->steering );
}

TEST( MotionReset, it_fails_if_no_agent_exists )
{
    auto controller = crimild::alloc< BehaviorController >();
    auto behavior = crimild::alloc< MotionReset >();

    EXPECT_EQ( Behavior::State::FAILURE, behavior->step( controller->getContext() ) );
}

TEST( MotionReset, it_fails_if_executed_without_init )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MotionReset >();

    EXPECT_EQ( Behavior::State::FAILURE, behavior->step( controller->getContext() ) );
}

TEST( MotionReset, it_succeeds )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MotionReset >();

    controller->execute( behavior );

    EXPECT_EQ( Behavior::State::SUCCESS, behavior->step( controller->getContext() ) );
}

TEST( MotionReset, it_resets_motion_state_on_step )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    node->setLocal( translation( 10, 20, 30 ) );

    auto motion = node->attachComponent< MotionState >();
    motion->steering = Vector3 { 7, 8, 9 };

    controller->execute( crimild::alloc< MotionReset >() );

    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
    EXPECT_EQ( ( Point3f { 0, 0, 0 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 7, 8, 9 } ), motion->steering );

    controller->update( Clock {} );

    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
    EXPECT_EQ( ( Point3f { 10, 20, 30 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
}
