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
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Foundation/Variant.hpp"

#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"

#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <vector>

using namespace crimild;

TEST( Variant, rtti )
{
    EXPECT_EQ( "crimild::Variant", Variant::__CLASS_NAME );

    const auto var = Variant();
    EXPECT_EQ( "crimild::Variant", var.getClassName() );
}

TEST( Variant, is_valid )
{
    const auto var = Variant();
    EXPECT_FALSE( var.isValid() );
}

TEST( Variant, from_int )
{
    const auto var = Variant( 10 );

    EXPECT_EQ( var.get< int >(), 10 );
    EXPECT_TRUE( var.isValid() );
}

TEST( Variant, from_struct )
{
    struct Foo {
        int x;
        int y;
    };

    const auto var = Variant( Foo { 20, 30 } );
    EXPECT_EQ( 20, var.get< Foo >().x );
    EXPECT_EQ( 30, var.get< Foo >().y );
}

TEST( Variant, update_value )
{
    auto var = Variant( 10 );
    EXPECT_EQ( var.get< int >(), 10 );
    var.get< int >() = 20;
    EXPECT_EQ( 20, var.get< int >() );
}

TEST( Variant, update_value_from_reference )
{
    auto var = Variant( 10 );
    EXPECT_EQ( var.get< int >(), 10 );
    auto &value = var.get< int >();
    value = 20;
    EXPECT_EQ( 20, var.get< int >() );
}

TEST( Variant, override_value )
{
    const auto v0 = Variant( 10 );
    auto v1 = Variant( 15 );
    v1 = v0;

    EXPECT_EQ( 10, v1.get< int >() );
}

TEST( Variant, can_be_moved )
{
    auto v0 = Variant( 10 );
    auto v1 = std::move( v0 );

    EXPECT_FALSE( v0.isValid() );
    EXPECT_TRUE( v1.isValid() );
    EXPECT_EQ( 10, v1.get< int >() );

    auto v2 = Variant();
    v2 = std::move( v1 );

    EXPECT_FALSE( v1.isValid() );
    EXPECT_TRUE( v2.isValid() );
    EXPECT_EQ( 10, v2.get< int >() );
}

TEST( Variant, copy_different )
{
    struct Foo {
        int x;
        int y;
    };

    const auto v0 = Variant( Foo { 20, 30 } );
    auto v1 = Variant( 10 );
    v1 = v0;

    EXPECT_EQ( 20, v1.get< Foo >().x );
    EXPECT_EQ( 30, v1.get< Foo >().y );
}

TEST( Variant, works_on_vector )
{
    struct Foo {
        int x;
        int y;
    };

    std::vector< Variant > vars = {
        Variant( 10 ),
        Variant( Foo { 20, 30 } ),
    };

    EXPECT_EQ( 10, vars[ 0 ].get< int >() );
    vars[ 0 ].get< int >() = 20;
    EXPECT_EQ( 20, vars[ 0 ].get< int >() );

    EXPECT_EQ( 20, vars[ 1 ].get< Foo >().x );
    EXPECT_EQ( 30, vars[ 1 ].get< Foo >().y );
}

TEST( Variant, degenarate_case )
{
    const auto var = Variant( 10 );

    EXPECT_NE( 10, var.get< float >() );
}

TEST( Variant, works_on_map )
{
    struct Foo {
        int x;
        int y;
    };

    std::unordered_map< std::string, Variant > vars = {
        { "one", Variant( 10 ) },
        { "two", Variant( Foo { 20, 30 } ) },
    };

    EXPECT_EQ( 10, vars[ "one" ].get< int >() );

    EXPECT_EQ( 20, vars[ "two" ].get< Foo >().x );
    EXPECT_EQ( 30, vars[ "two" ].get< Foo >().y );
}

TEST( Variant, coding )
{
    struct Foo {
        int x;
        int y;
    };

    const auto v0 = crimild::alloc< Variant >( Foo { 20, 30 } );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    encoder->encode( v0 );
    auto bytes = encoder->getBytes();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();
    decoder->fromBytes( bytes );

    const auto v1 = decoder->getObjectAt< Variant >( 0 );
    EXPECT_EQ( 20, v1->get< Foo >().x );
    EXPECT_EQ( 30, v1->get< Foo >().y );
}
