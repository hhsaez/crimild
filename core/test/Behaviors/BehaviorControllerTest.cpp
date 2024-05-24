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

#include "Behaviors/BehaviorController.hpp"

#include "Behaviors/Actions/SetContextValue.hpp"
#include "Behaviors/BehaviorTree.hpp"
#include "Behaviors/Decorators/Repeat.hpp"
#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Crimild_Foundation.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"

#include "gtest/gtest.h"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;
using namespace crimild::behaviors::decorators;

namespace crimild {

    namespace behaviors {

        class MockBehavior : public Behavior {
            CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::MockBehavior )

        public:
            virtual void init( BehaviorContext *context ) noexcept override
            {
                if ( !context->has( "initCount" ) ) {
                    m_initCount = context->set( "initCount", uint32_t( 1 ) );
                } else {
                    m_initCount = context->get( "initCount" );
                    m_initCount->get< uint32_t >() += 1;
                }

                if ( !context->has( "executeCount" ) ) {
                    m_executeCount = context->set( "executeCount", uint32_t( 0 ) );
                } else {
                    m_executeCount = context->get( "executeCount" );
                }
            }

            virtual Behavior::State step( BehaviorContext * ) noexcept override
            {
                if ( m_executeCount == nullptr ) {
                    CRIMILD_LOG_WARNING( "Not initialized" );
                    return Behavior::State::FAILURE;
                }

                m_executeCount->get< uint32_t >() += 1;
                return Behavior::State::SUCCESS;
            }

            void reset( void ) noexcept
            {
                m_initCount->get< uint32_t >() = 0;
                m_executeCount->get< uint32_t >() = 0;
            }

        private:
            SharedPointer< Variant > m_initCount;
            SharedPointer< Variant > m_executeCount;
        };
    }

}

TEST( BehaviorController, construction )
{
    auto controller = crimild::alloc< BehaviorController >();

    ASSERT_NE( nullptr, controller->getContext() );

    ASSERT_STREQ(
        "crimild::behaviors::actions::Success",
        controller->getBehavior( BehaviorController::SCENE_STARTED_BEHAVIOR_NAME )->getClassName()
    );

    ASSERT_STREQ(
        "crimild::behaviors::actions::Success",
        controller->getBehavior( BehaviorController::DEFAULT_BEHAVIOR_NAME )->getClassName()
    );

    Clock c;
    controller->update( c ); // Just works
}

TEST( BehaviorController, it_does_not_intializes_behavior_when_if_not_attached )
{
    auto controller = crimild::alloc< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior );

    ASSERT_FALSE( controller->getContext()->has( "initCount" ) );
}

TEST( BehaviorController, it_intializes_behavior_when_calling_execute_if_started )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior );

    ASSERT_TRUE( controller->getContext()->has( "initCount" ) );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
}

TEST( BehaviorController, it_intializes_behavior_twice_if_forced )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior, true );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );

    // Initialized again
    controller->execute( behavior, true );
    EXPECT_EQ( 2, controller->getContext()->get( "initCount" )->get< uint32_t >() );
}

TEST( BehaviorController, it_may_intialize_behavior_twice_during_start )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior, true );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );

    // Initialized again
    controller->start();
    EXPECT_EQ( 2, controller->getContext()->get( "initCount" )->get< uint32_t >() );
}

TEST( BehaviorController, it_does_not_intializes_behavior_twice_if_active )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );

    // Not reinitialized since it's still active
    controller->execute( behavior );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
}

TEST( BehaviorController, it_deyals_behavior_execution_until_update )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 0, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    controller->update( Clock {} );
    EXPECT_EQ( 1, controller->getContext()->get( "executeCount" )->get< uint32_t >() );
}

TEST( BehaviorController, it_executes_behavior_only_once )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 0, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    controller->update( Clock {} );
    EXPECT_EQ( 1, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    // No longer executed since it already terminated
    controller->update( Clock {} );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
}

TEST( BehaviorController, it_calls_init_again_after_execution )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 0, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    controller->update( Clock {} );
    EXPECT_EQ( 1, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    // Re-initialized since it's no longer active
    controller->execute( behavior );
    EXPECT_EQ( 2, controller->getContext()->get( "initCount" )->get< uint32_t >() );
}

TEST( BehaviorController, it_executes_again_after_reinit )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto behavior = crimild::alloc< MockBehavior >();

    controller->execute( behavior );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 0, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    controller->update( Clock {} );
    EXPECT_EQ( 1, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    // Reinit
    controller->execute( behavior );
    EXPECT_EQ( 2, controller->getContext()->get( "initCount" )->get< uint32_t >() );

    // Executes again
    controller->update( Clock {} );
    EXPECT_EQ( 2, controller->getContext()->get( "executeCount" )->get< uint32_t >() );
}

TEST( BehaviorController, it_executes_multiple_times_with_repeat )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto mock = crimild::alloc< MockBehavior >();
    auto repeat = crimild::alloc< Repeat >();
    repeat->setBehavior( mock );

    controller->execute( repeat );
    EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 0, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    // MockBehavior is always reinit-ed in each loop of Repeat
    controller->update( Clock {} );
    EXPECT_EQ( 2, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 1, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    controller->update( Clock {} );
    EXPECT_EQ( 3, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 2, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    controller->update( Clock {} );
    EXPECT_EQ( 4, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 3, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

    controller->update( Clock {} );
    EXPECT_EQ( 5, controller->getContext()->get( "initCount" )->get< uint32_t >() );
    EXPECT_EQ( 4, controller->getContext()->get( "executeCount" )->get< uint32_t >() );
}

TEST( BehaviorController, coding )
{
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::MockBehavior );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();

    {
        // Create a controller and execute it a couple of times
        auto node = crimild::alloc< Node >();
        auto controller = node->attachComponent< BehaviorController >();
        auto mock = crimild::alloc< MockBehavior >();
        auto repeat = crimild::alloc< Repeat >();
        repeat->setBehavior( mock );

        controller->execute( repeat );
        EXPECT_EQ( 1, controller->getContext()->get( "initCount" )->get< uint32_t >() );
        EXPECT_EQ( 0, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

        controller->update( Clock {} );
        EXPECT_EQ( 2, controller->getContext()->get( "initCount" )->get< uint32_t >() );
        EXPECT_EQ( 1, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

        controller->update( Clock {} );
        EXPECT_EQ( 3, controller->getContext()->get( "initCount" )->get< uint32_t >() );
        EXPECT_EQ( 2, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

        // Encoding
        encoder->encode( node );
    }

    {
        // Decoding
        decoder->fromBytes( encoder->getBytes() );

        auto node = decoder->getObjectAt< Node >( 0 );
        ASSERT_NE( nullptr, node );

        auto controller = node->getComponent< BehaviorController >();
        ASSERT_NE( nullptr, controller );

        // Don't forget to start controller
        controller->start();

        // Check initial values
        ASSERT_TRUE( controller->getContext()->has( "initCount" ) );
        // Init is called again during controller decoding. So the counter is incremented.
        EXPECT_EQ( 4, controller->getContext()->get( "initCount" )->get< uint32_t >() );
        // But execute counter remains the same
        ASSERT_TRUE( controller->getContext()->has( "executeCount" ) );
        EXPECT_EQ( 2, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

        // Execute again
        controller->update( Clock {} );
        EXPECT_EQ( 5, controller->getContext()->get( "initCount" )->get< uint32_t >() );
        EXPECT_EQ( 3, controller->getContext()->get( "executeCount" )->get< uint32_t >() );

        controller->update( Clock {} );
        EXPECT_EQ( 6, controller->getContext()->get( "initCount" )->get< uint32_t >() );
        EXPECT_EQ( 4, controller->getContext()->get( "executeCount" )->get< uint32_t >() );
    }
}

TEST( BehaviorController, coding_with_target )
{
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::behaviors::MockBehavior );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();

    {
        auto group = crimild::alloc< Group >();

        // Create a controller and execute it a couple of times
        auto node = crimild::alloc< Node >();
        auto controller = node->attachComponent< BehaviorController >();
        group->attachNode( node );

        auto target = crimild::alloc< Node >( "a target" );
        controller->getContext()->addTarget( get_ptr( target ) );
        group->attachNode( target );

        encoder->encode( group );
    }

    {
        // Decoding
        decoder->fromBytes( encoder->getBytes() );

        ASSERT_EQ( 1, decoder->getObjectCount() );

        auto group = decoder->getObjectAt< Group >( 0 );
        ASSERT_NE( nullptr, group );

        auto node = group->getNodeAt( 0 );
        ASSERT_NE( nullptr, node );

        auto controller = node->getComponent< BehaviorController >();
        ASSERT_NE( nullptr, controller );

        auto context = controller->getContext();
        ASSERT_NE( nullptr, context );

        auto target = group->getNodeAt( 1 );
        ASSERT_NE( nullptr, target );
        ASSERT_EQ( "a target", target->getName() );

        ASSERT_TRUE( context->hasTargets() );
        ASSERT_EQ( 1, context->getTargetCount() );
        ASSERT_EQ( get_ptr( target ), context->getTargetAt( 0 ) );
        ASSERT_EQ( "a target", context->getTargetAt( 0 )->getName() );
    }
}
