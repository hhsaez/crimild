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

#include "SceneGraph/Node.hpp"
#include "Coding/LuaEncoder.hpp"
#include "Coding/LuaDecoder.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( NodeTest, luaCoding )
{
    auto n1 = crimild::alloc< Node >( "Some node" );
    n1->local().setTranslate( 10, 20, 30 );
    n1->world().setTranslate( 50, 70, 90 );
    n1->setWorldIsCurrent( true );
    
    auto encoder = crimild::alloc< coding::LuaEncoder >();
    encoder->encode( n1 );
    auto encoded = "scene = " + encoder->getEncodedString();
    auto decoder = crimild::alloc< coding::LuaDecoder >();
    decoder->parse( encoded );
    
    auto n2 = decoder->getObjectAt< Node >( 0 );
    EXPECT_TRUE( n2 != nullptr );
    EXPECT_EQ( n1->getName(), n2->getName() );
    EXPECT_EQ( n1->getLocal().getTranslate(), n2->getLocal().getTranslate() );
    EXPECT_EQ( n1->getWorld().getTranslate(), n2->getWorld().getTranslate() );
    EXPECT_EQ( n1->worldIsCurrent(), n2->worldIsCurrent() );
}

TEST( NodeTest, luaCodingTransformation )
{
    auto n1 = crimild::alloc< Node >( "Some Node" );
    n1->local().setTranslate( Vector3f( 0.0f, 0.0f, -5.0f ) );
    n1->local().setRotate( Vector3f( 0.0f, 1.0f, 0.0f ), Numericf::PI );
    n1->local().setScale( 0.5f );
    
    auto encoder = crimild::alloc< coding::LuaEncoder >();
    encoder->encode( n1 );
    auto encoded = "scene = " + encoder->getEncodedString();
    auto decoder = crimild::alloc< coding::LuaDecoder >();
    decoder->parse( encoded );
    
    auto n2 = decoder->getObjectAt< Node >( 0 );
    EXPECT_TRUE( n2 != nullptr );
    EXPECT_EQ( n1->getName(), n2->getName() );
    EXPECT_EQ( n1->getLocal().getTranslate(), n2->getLocal().getTranslate() );
    EXPECT_EQ( n1->getLocal().getRotate(), n2->getLocal().getRotate() );
    EXPECT_EQ( n1->getLocal().getScale(), n2->getLocal().getScale() );
}

