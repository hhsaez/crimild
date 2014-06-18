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

#include "Foundation/ScriptContext.hpp"

#include "gtest/gtest.h"

using namespace crimild;
using namespace crimild::scripting;

int my_add( int a, int b ) {
    return a + b;
}

TEST( ScriptContextTest, functionNoArgs )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    l.invoke( "foo" );
}

TEST( ScriptContextTest, add )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    EXPECT_EQ ( 7, l.invoke< int >( "add", 5, 2 ) );
}

TEST ( ScriptContextTest, oneArgument )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    l.invoke( "oneArgument", 1 );
}

TEST ( ScriptContextTest, oneReturn )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    EXPECT_EQ ( 1, l.invoke< int >( "oneReturn" ) );
}

TEST ( ScriptContextTest, oneArgumentOneReturn )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    EXPECT_EQ ( 7, l.invoke< int >( "oneArgumentOneReturn", 7 ) );
}

TEST( ScriptContextTest, multiReturn )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

    int sum, difference;
    std::tie( sum, difference ) = l.invoke<int, int>( "sum_and_difference", 3, 1 );
    EXPECT_EQ( 4, sum );
    EXPECT_EQ( 2, difference );
}

TEST( ScriptContextTest, heterogeneousReturn )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

    int x;
    bool y;
    std::string z;
    std::tie( x, y, z ) = l.invoke< int, bool, std::string >( "bar" );
    EXPECT_EQ( 4, x );
    EXPECT_EQ( true, y );
    EXPECT_EQ( "hi", z );
}

TEST( ScriptContextTest, callCFunction )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

    l.registerFunction( "cadd", std::function< int( int, int ) >( my_add ) );
    int answer = l.invoke< int >( "cadd", 3, 6 );

    EXPECT_EQ( 9, answer );
}

TEST( ScriptContextTest, callCFunctionFromLua )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

    l.registerFunction( "cadd", std::function< int( int, int ) >( my_add ) );
    int answer = l.invoke< int >( "execute" );

    EXPECT_EQ( 11, answer );
}

std::tuple< int, int > my_sum_and_difference( int x, int y ) 
{
 	return std::make_tuple( x + y, x - y );
}

TEST ( ScriptContextTest, multivalueCFunctionReturn )
{
	ScriptContext l;
	l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	l.registerFunction( "test_fun", &my_sum_and_difference );

	int sum, difference;
	std::tie( sum, difference ) = l.invoke< int, int >( "test_fun", -2, 2 );

	EXPECT_EQ( 0, sum );
	EXPECT_EQ( -4, difference );
}

TEST ( ScriptContextTest, multivalueCFuncFromLua )
{
	ScriptContext l;
	l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	l.registerFunction( "doozy_c", &my_sum_and_difference );

	EXPECT_EQ( false, l.isNil( "doozy_c" ) );

	int answer = l.invoke< int >( "doozy", 5 );

	EXPECT_EQ( -75, answer );
}

TEST ( ScriptContextTest, cFuncDestructor )
{
	ScriptContext l;
	l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	l.registerFunction( "doozy_c", &my_sum_and_difference );

	EXPECT_EQ( false, l.isNil( "doozy_c" ) );

	l.invoke< int >( "doozy", 5 );

	l.unregisterFunction( "doozy_c" );

	EXPECT_EQ( true, l.isNil( "doozy_c" ) );
}

TEST ( ScriptContextTest, values )
{
	ScriptContext l;
	l.load( FileSystem::getInstance().pathForResource( "Scripts/player.lua" ) );

	EXPECT_EQ( 10, l.getValue< float >( "player.position.x" ) );
	EXPECT_EQ( 30, l.getValue< float >( "player.position.y" ) );
	EXPECT_EQ( "player", l.getValue< std::string >( "player.name" ) );
	EXPECT_EQ( "warrior.obj", l.getValue< std::string >( "player.model" ) );
}

TEST ( ScriptContextTest, defaultValues )
{
	ScriptContext l;
	l.load( FileSystem::getInstance().pathForResource( "Scripts/player.lua" ) );

	EXPECT_EQ( -1, l.getValue< float >( "player.position.z" ) );
	EXPECT_EQ( "null", l.getValue< std::string >( "player.background" ) );
}

