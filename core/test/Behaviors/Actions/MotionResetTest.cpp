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
#include "Mathematics/Transformation_translation.hpp"
#include "SceneGraph/Node.hpp"

#include <gtest/gtest.h>

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

TEST( MotionReset, it_set_default_values_to_context_on_init )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    EXPECT_FALSE( controller->getContext()->has( "motion.velocity" ) );
    EXPECT_FALSE( controller->getContext()->has( "motion.position" ) );
    EXPECT_FALSE( controller->getContext()->has( "motion.steering" ) );

    controller->execute( crimild::alloc< MotionReset >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.velocity" ) );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), controller->getContext()->get( "motion.velocity" )->get< Vector3 >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.position" ) );
    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), controller->getContext()->get( "motion.position" )->get< Point3 >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.steering" ) );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), controller->getContext()->get( "motion.steering" )->get< Vector3 >() );
}

TEST( MotionReset, it_does_not_modify_existing_values_on_init )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    controller->getContext()->set( "motion.velocity", Vector3 { 1, 2, 3 } );
    controller->getContext()->set( "motion.position", Point3 { 4, 5, 6 } );
    controller->getContext()->set( "motion.steering", Vector3 { 7, 8, 9 } );

    controller->execute( crimild::alloc< MotionReset >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.velocity" ) );
    EXPECT_EQ( ( Vector3 { 1, 2, 3 } ), controller->getContext()->get( "motion.velocity" )->get< Vector3 >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.position" ) );
    EXPECT_EQ( ( Point3 { 4, 5, 6 } ), controller->getContext()->get( "motion.position" )->get< Point3 >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.steering" ) );
    EXPECT_EQ( ( Vector3 { 7, 8, 9 } ), controller->getContext()->get( "motion.steering" )->get< Vector3 >() );
}

TEST( MotionReset, it_reset_motion_state_on_step )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();

    node->setLocal( translation( 10, 20, 30 ) );
    controller->getContext()->set( "motion.steering", Vector3 { 7, 8, 9 } );

    controller->execute( crimild::alloc< MotionReset >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.velocity" ) );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), controller->getContext()->get( "motion.velocity" )->get< Vector3 >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.position" ) );
    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), controller->getContext()->get( "motion.position" )->get< Point3 >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.steering" ) );
    EXPECT_EQ( ( Vector3 { 7, 8, 9 } ), controller->getContext()->get( "motion.steering" )->get< Vector3 >() );

    controller->update( Clock {} );

    EXPECT_TRUE( controller->getContext()->has( "motion.velocity" ) );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), controller->getContext()->get( "motion.velocity" )->get< Vector3 >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.position" ) );
    EXPECT_EQ( ( Point3 { 10, 20, 30 } ), controller->getContext()->get( "motion.position" )->get< Point3 >() );

    EXPECT_TRUE( controller->getContext()->has( "motion.steering" ) );
    EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), controller->getContext()->get( "motion.steering" )->get< Vector3 >() );
}

TEST( MotionReset, it_fails_if_no_agent_exists )
{
    auto controller = crimild::alloc< BehaviorController >();
    auto behavior = crimild::alloc< MotionReset >();

    EXPECT_EQ( Behavior::State::FAILURE, behavior->step( controller->getContext() ) );
}
