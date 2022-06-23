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

#include "Behaviors/Actions/MotionApply.hpp"

#include "Behaviors/Actions/MotionReset.hpp"
#include "Behaviors/BehaviorController.hpp"
#include "Behaviors/Composites/Sequence.hpp"
#include "Behaviors/Decorators/Repeat.hpp"
#include "Components/MotionStateComponent.hpp"
#include "Mathematics/Point_equality.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "Mathematics/Vector_equality.hpp"
#include "SceneGraph/Node.hpp"

#include <gtest/gtest.h>

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;
using namespace crimild::behaviors::composites;
using namespace crimild::behaviors::decorators;

TEST( MotionApply, it_attaches_motion_state_component_on_init_if_not_present )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    ASSERT_EQ( nullptr, node->getComponent< MotionState >() );

    controller->execute( crimild::alloc< MotionApply >() );

    ASSERT_NE( nullptr, node->getComponent< MotionState >() );
}

TEST( MotionApply, it_does_not_modify_existing_state_on_init_if_already_present )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    auto motion = node->attachComponent< MotionState >();
    motion->velocity = Vector3 { 1, 2, 3 };
    motion->position = Point3 { 4, 5, 6 };
    motion->steering = Vector3 { 7, 8, 9 };

    controller->execute( crimild::alloc< MotionApply >() );

    ASSERT_NE( nullptr, node->getComponent< MotionState >() );
    EXPECT_EQ( ( Vector3 { 1, 2, 3 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 4, 5, 6 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 7, 8, 9 } ), motion->steering );
}

TEST( MotionApply, it_fails_if_no_agent_exists )
{
    auto controller = crimild::alloc< BehaviorController >();
    auto behavior = crimild::alloc< MotionApply >();

    EXPECT_EQ( Behavior::State::FAILURE, behavior->step( controller->getContext() ) );
}

TEST( MotionApply, it_fails_if_executed_before_init )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MotionApply >();

    EXPECT_EQ( Behavior::State::FAILURE, behavior->step( controller->getContext() ) );
}

TEST( MotionApply, it_succeeds )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MotionApply >();

    controller->execute( behavior );

    EXPECT_EQ( Behavior::State::SUCCESS, behavior->step( controller->getContext() ) );
}

TEST( MotionApply, it_overrides_current_actor_location_if_reset_not_used )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    node->setLocal( translation( 1, 2, 3 ) );

    controller->execute( crimild::alloc< MotionApply >() );

    // not changed until update()
    EXPECT_EQ( ( Point3 { 1, 2, 3 } ), location( node->getLocal() ) );

    controller->update( Clock( 0 ) ); // update with no time diff

    // location overriden
    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), location( node->getLocal() ) );
}

TEST( MotionApply, it_keeps_current_actor_location_if_reset_is_used )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    node->setLocal( translation( 1, 2, 3 ) );

    controller->execute(
        [] {
            auto sequence = crimild::alloc< Sequence >();
            sequence->attachBehavior( crimild::alloc< MotionReset >() );
            sequence->attachBehavior( crimild::alloc< MotionApply >() );
            return sequence;
        }()
    );

    // not changed until update()
    EXPECT_EQ( ( Point3 { 1, 2, 3 } ), location( node->getLocal() ) );

    controller->update( Clock( 0 ) ); // update with no time diff

    // location does not changes
    EXPECT_EQ( ( Point3 { 1, 2, 3 } ), location( node->getLocal() ) );
}

TEST( MotionApply, it_applies_steering_without_mass )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    controller->execute( crimild::alloc< MotionApply >() );

    auto motion = node->getComponent< MotionState >();
    motion->mass = 0;               // Force to 0
    motion->velocity = { 0, 1, 0 }; // Previous velocity
    motion->steering = { 0, 0, 1 };

    controller->update( Clock( 1 ) );

    // Motion is instantaenous based only on steering vector
    EXPECT_EQ( ( Vector3 { 0, 0, 1 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, 0, 1 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 1 } ), motion->steering );
}

TEST( MotionApply, it_applies_steering_with_mass )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    controller->execute( crimild::alloc< MotionApply >() );

    auto motion = node->getComponent< MotionState >();
    motion->mass = 1;               // Force to 1
    motion->velocity = { 0, 1, 0 }; // Previous velocity
    motion->steering = { 0, 0, 1 };

    controller->update( Clock( 1 ) );

    // Motion accounts for previous velocity, which is different than steering vector
    EXPECT_EQ( ( Vector3 { 0, numbers::SQRT_2_DIV_2, numbers::SQRT_2_DIV_2 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, numbers::SQRT_2_DIV_2, numbers::SQRT_2_DIV_2 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 1 } ), motion->steering );
}

TEST( MotionApply, integration )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    controller->execute(
        [] {
            auto repeat = crimild::alloc< Repeat >();
            repeat->setBehavior(
                [] {
                    auto sequence = crimild::alloc< Sequence >();
                    sequence->attachBehavior( crimild::alloc< MotionReset >() );
                    sequence->attachBehavior( crimild::alloc< MotionApply >() );
                    return sequence;
                }()
            );
            return repeat;
        }()
    );

    auto motion = node->getComponent< MotionState >();
    ASSERT_NE( nullptr, motion );

    // Set some mass for motion, so velocity is accumulated
    motion->mass = 1.0;

    // Actor position not modified until update
    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), location( node->getLocal() ) );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );

    controller->update( Clock( 1 ) );

    // Nothing happens because there's no velocity
    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), location( node->getLocal() ) );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );

    // Set a constant velocity before update
    motion->velocity = Vector3::Constants::FORWARD;

    controller->update( Clock( 1 ) );

    // Actor position update
    EXPECT_EQ( ( Point3 { 0, 0, -1 } ), location( node->getLocal() ) );
    EXPECT_EQ( ( Vector3 { 0, 0, -1 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, 0, -1 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );

    controller->update( Clock( 1 ) );

    // Actor position update
    EXPECT_EQ( ( Point3 { 0, 0, -2 } ), location( node->getLocal() ) );
    EXPECT_EQ( ( Vector3 { 0, 0, -1 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, 0, -2 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );

    controller->update( Clock( 1 ) );

    // Actor position update
    EXPECT_EQ( ( Point3 { 0, 0, -3 } ), location( node->getLocal() ) );
    EXPECT_EQ( ( Vector3 { 0, 0, -1 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, 0, -3 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );

    // Stop
    motion->velocity = Vector3::Constants::ZERO;

    controller->update( Clock( 1 ) );

    // Actor position remains the same
    EXPECT_EQ( ( Point3 { 0, 0, -3 } ), location( node->getLocal() ) );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
    EXPECT_EQ( ( Point3 { 0, 0, -3 } ), motion->position );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
}
