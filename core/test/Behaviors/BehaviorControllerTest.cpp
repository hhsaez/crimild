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
#include "Behaviors/BehaviorTree.hpp"
#include "Behaviors/Actions/SetContextValue.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/Settings.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Coding/MemoryDecoder.hpp"

#include "gtest/gtest.h"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

TEST( BehaviorControllerTest, setContextValue )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto context = controller->getContext();
    context->setValue( "input.x", 100 );
    controller->attachBehaviorTree( crimild::alloc< BehaviorTree >( "initInputX", crimild::alloc< SetContextValue >( "input.x", "20" ) ) );
    controller->attachBehaviorTree( crimild::alloc< BehaviorTree >( "updateInputX", crimild::alloc< SetContextValue >( "input.x", "30" ) ) );

    EXPECT_EQ( 100, context->getValue< crimild::Int32 >( "input.x" ) );

    controller->getBehaviorTree( "initInputX" )->getRootBehavior()->step( controller->getContext() );
    EXPECT_EQ( 20, context->getValue< crimild::Int32 >( "input.x" ) );

    controller->getBehaviorTree( "updateInputX" )->getRootBehavior()->step( controller->getContext() );
    EXPECT_EQ( 30, context->getValue< crimild::Int32 >( "input.x" ) );
}

TEST( BehaviorControllerTest, coding )
{
    auto node = crimild::alloc< Node >();
    auto controller = node->attachComponent< BehaviorController >();
    auto context = controller->getContext();
    context->setValue( "input.x", 100 );
    controller->attachBehaviorTree( crimild::alloc< BehaviorTree >( "initInputX", crimild::alloc< SetContextValue >( "input.x", "20" ) ) );
    controller->attachBehaviorTree( crimild::alloc< BehaviorTree >( "updateInputX", crimild::alloc< SetContextValue >( "input.x", "30" ) ) );

    EXPECT_EQ( 100, context->getValue< crimild::Int32 >( "input.x" ) );

    controller->getBehaviorTree( "initInputX" )->getRootBehavior()->step( controller->getContext() );
    EXPECT_EQ( 20, context->getValue< crimild::Int32 >( "input.x" ) );

    controller->getBehaviorTree( "updateInputX" )->getRootBehavior()->step( controller->getContext() );
    EXPECT_EQ( 30, context->getValue< crimild::Int32 >( "input.x" ) );

	auto encoder = crimild::alloc< coding::MemoryEncoder >();
	encoder->encode( node );
	auto bytes = encoder->getBytes();
	auto decoder = crimild::alloc< coding::MemoryDecoder >();
	decoder->fromBytes( bytes );

	auto decodedNode = decoder->getObjectAt< Node >( 0 );
	EXPECT_TRUE( decodedNode != nullptr );
    auto decodedBehaviors = decodedNode->getComponent< BehaviorController >();
    EXPECT_TRUE( decodedBehaviors != nullptr );
    auto decodedContext = decodedBehaviors->getContext();
    EXPECT_TRUE( decodedContext != nullptr );
    EXPECT_EQ( 30, decodedContext->getValue< crimild::Int32 >( "input.x" ) );
    decodedBehaviors->getBehaviorTree( "initInputX" )->getRootBehavior()->step( decodedContext );
    EXPECT_EQ( 20, decodedContext->getValue< crimild::Int32 >( "input.x" ) );
    decodedBehaviors->getBehaviorTree( "updateInputX" )->getRootBehavior()->step( decodedContext );
    EXPECT_EQ( 30, decodedContext->getValue< crimild::Int32 >( "input.x" ) );
}

