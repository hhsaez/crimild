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

#include "Behaviors/BehaviorContext.hpp"

#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "SceneGraph/Node.hpp"

#include "gtest/gtest.h"

using namespace crimild;
using namespace crimild::behaviors;

TEST( BehaviorContext, can_set_values )
{
    auto context = crimild::alloc< BehaviorContext >();

    EXPECT_FALSE( context->has( "value" ) );

    context->set( "value", Int32( 20 ) );
    EXPECT_TRUE( context->has( "value" ) );
    EXPECT_TRUE( context->get( "value" )->isValid() );
    EXPECT_EQ( 20, context->get( "value" )->get< Int32 >() );
}

TEST( BehaviorContext, works_with_variants )
{
    auto context = crimild::alloc< BehaviorContext >();

    EXPECT_FALSE( context->has( "value" ) );

    context->set( "value", crimild::alloc< Variant >( Int32( 20 ) ) );
    EXPECT_TRUE( context->has( "value" ) );
    EXPECT_TRUE( context->get( "value" )->isValid() );
    EXPECT_EQ( 20, context->get( "value" )->get< Int32 >() );
}

TEST( BehaviorContext, get_or_default )
{
    auto context = crimild::alloc< BehaviorContext >();

    auto var = context->getOrCreate( "value", Int32( 42 ) );
    EXPECT_TRUE( context->has( "value" ) );
    EXPECT_TRUE( context->get( "value" )->isValid() );
    EXPECT_EQ( 42, context->get( "value" )->get< Int32 >() );
    EXPECT_EQ( 42, var->get< Int32 >() );
}

TEST( BehaviorContext, target_cannot_be_equalt_to_agent )
{
    auto node = crimild::alloc< Node >();

    auto context = crimild::alloc< BehaviorContext >();
    context->setAgent( get_ptr( node ) );
    context->addTarget( get_ptr( node ) );

    ASSERT_EQ( 0, context->getTargetCount() );
}

TEST( BehaviorContext, coding )
{
    auto context = crimild::alloc< BehaviorContext >();
    context->set( "input.x", crimild::alloc< Variant >( Int32( 10 ) ) );
    context->set( "input.y", crimild::alloc< Variant >( Int32( 20 ) ) );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    encoder->encode( context );
    auto bytes = encoder->getBytes();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();
    decoder->fromBytes( bytes );

    auto decodedContext = decoder->getObjectAt< BehaviorContext >( 0 );
    EXPECT_TRUE( decodedContext != nullptr );
    EXPECT_EQ( 10, decodedContext->get( "input.x" )->get< crimild::Int32 >() );
    EXPECT_EQ( 20, decodedContext->get( "input.y" )->get< crimild::Int32 >() );
}

TEST( BehaviorContext, coding_with_target )
{
    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();

    {
        auto context = crimild::alloc< BehaviorContext >();

        auto target = crimild::alloc< Node >();
        target->setName( "some target" );
        context->addTarget( get_ptr( target ) );

        encoder->encode( context );
    }

    {
        auto bytes = encoder->getBytes();
        decoder->fromBytes( bytes );

        auto decoded = decoder->getObjectAt< BehaviorContext >( 0 );
        ASSERT_TRUE( decoded != nullptr );

        ASSERT_TRUE( decoded->hasTargets() );
        ASSERT_EQ( 1, decoded->getTargetCount() );
        EXPECT_EQ( "some target", decoded->getTargetAt( 0 )->getName() );
    }
}
