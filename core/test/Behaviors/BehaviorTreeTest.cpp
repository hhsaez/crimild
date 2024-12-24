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

#include "Behaviors/BehaviorTree.hpp"

#include "Behaviors/Actions/SetContextValue.hpp"

#include <Crimild_Coding.hpp>
#include <gtest/gtest.h>

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

TEST( BehaviorTreeTest, coding )
{
    auto tree = crimild::alloc< BehaviorTree >();
    tree->setName( "behavior" );
    tree->setRootBehavior( crimild::alloc< SetContextValue >( "input.x", "20" ) );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    encoder->encode( tree );
    auto bytes = encoder->getBytes();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();
    decoder->fromBytes( bytes );

    auto decodedTree = decoder->getObjectAt< BehaviorTree >( 0 );
    EXPECT_TRUE( decodedTree != nullptr );
    EXPECT_EQ( tree->getName(), decodedTree->getName() );
    EXPECT_TRUE( decodedTree->getRootBehavior() );
}
