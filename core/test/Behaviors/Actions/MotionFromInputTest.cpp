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

#include "Behaviors/Actions/MotionFromInput.hpp"

#include "Behaviors/Actions/MotionApply.hpp"
#include "Behaviors/Actions/MotionReset.hpp"
#include "Behaviors/Composites/Sequence.hpp"
#include "Behaviors/Decorators/Repeat.hpp"
#include "Behaviors/withBehavior.hpp"
#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Components/MotionStateComponent.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Event.hpp"
#include "Simulation/Input.hpp"

#include <gtest/gtest.h>

namespace crimild {

    namespace behaviors {

        namespace actions {

            auto motionReset( void ) noexcept
            {
                return crimild::alloc< MotionReset >();
            }

            auto motionFromInput( void ) noexcept
            {
                return crimild::alloc< MotionFromInput >();
            }

            auto motionApply( void ) noexcept
            {
                return crimild::alloc< MotionApply >();
            }

        }

        namespace composites {

            auto sequence( std::initializer_list< SharedPointer< Behavior > > bs ) noexcept
            {
                auto ret = crimild::alloc< Sequence >();
                for ( auto &b : bs ) {
                    ret->attachBehavior( b );
                }
                return ret;
            }

        }

        namespace decorators {

            auto repeat( auto behavior ) noexcept
            {
                auto ret = crimild::alloc< Repeat >();
                ret->setBehavior( behavior );
                return ret;
            }
        }

    }

}

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;
using namespace crimild::behaviors::composites;
using namespace crimild::behaviors::decorators;

// TESTS START HERE!!

TEST( MotionFromInput, integration )
{
    auto node = behaviors::withBehavior(
        crimild::alloc< Node >(),
        repeat(
            sequence(
                {
                    motionReset(),
                    motionFromInput(),
                    motionApply(),
                }
            )
        )
    );

    Input input;

    auto controller = node->getComponent< BehaviorController >();
    auto motion = node->getComponent< MotionState >();

    // Helper functions
    auto keyEvent = []( auto type, auto key ) {
        return [ type, key ] {
            Input::getInstance()->handle(
                Event {
                    .type = type,
                    .keyboard {
                        .key = uint32_t( key ),
                    },
                }
            );
        };
    };
    auto pressW = keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_W );
    auto releaseW = keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_W );

    {
        // No input.

        controller->update( Clock( 1 ) );

        // Motion remains the same
        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 0, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
    }

    {
        node->setLocal( translation( 0, 0, 0 ) );

        // Press once and release
        pressW();
        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 1, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->steering );

        releaseW();
        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 1, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
    }

    {
        node->setLocal( translation( 0, 0, 0 ) );

        // Keep pressing W for several updates
        pressW();
        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 1, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->steering );

        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 2, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->steering );

        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 3, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->steering );

        releaseW();
        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 3, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
    }
}

TEST( MotionFromInput, coding_integration )
{
    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();

    {
        auto node = behaviors::withBehavior(
            crimild::alloc< Node >(),
            repeat(
                sequence(
                    {
                        motionReset(),
                        motionFromInput(),
                        motionApply(),
                    }
                )
            )
        );

        encoder->encode( node );
    }

    auto bytes = encoder->getBytes();
    decoder->fromBytes( bytes );

    auto node = decoder->getObjectAt< Node >( 0 );

    Input input;

    auto controller = node->getComponent< BehaviorController >();
    auto motion = node->getComponent< MotionState >();

    // Don't forget to start controller
    controller->start();

    // Helper functions
    auto keyEvent = []( auto type, auto key ) {
        return [ type, key ] {
            Input::getInstance()->handle(
                Event {
                    .type = type,
                    .keyboard {
                        .key = uint32_t( key ),
                    },
                }
            );
        };
    };
    auto pressW = keyEvent( Event::Type::KEY_DOWN, CRIMILD_INPUT_KEY_W );
    auto releaseW = keyEvent( Event::Type::KEY_UP, CRIMILD_INPUT_KEY_W );

    {
        // No input.

        controller->update( Clock( 1 ) );

        // Motion remains the same
        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 0, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
    }

    {
        node->setLocal( translation( 0, 0, 0 ) );

        // Press once and release
        pressW();
        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 1, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->steering );

        releaseW();
        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 1, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
    }

    {
        node->setLocal( translation( 0, 0, 0 ) );

        // Keep pressing W for several updates
        pressW();
        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 1, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->steering );

        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 2, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->steering );

        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 3, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 1, 0 } ), motion->steering );

        releaseW();
        controller->update( Clock( 1 ) );

        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->velocity );
        EXPECT_EQ( ( Point3 { 0, 3, 0 } ), motion->position );
        EXPECT_EQ( ( Vector3 { 0, 0, 0 } ), motion->steering );
    }
}
