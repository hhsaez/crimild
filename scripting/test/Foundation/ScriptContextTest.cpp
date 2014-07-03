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

TEST( ScriptContextTest, testFunctionNoArgs )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    l.invoke( "foo" );
}

TEST( ScriptContextTest, testAdd )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    EXPECT_EQ ( 7, l.invoke< int >( "add", 5, 2 ) );
}

TEST ( ScriptContextTest, testOneArgument )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    l.invoke( "oneArgument", 1 );
}

TEST ( ScriptContextTest, testOneReturn )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    EXPECT_EQ ( 1, l.invoke< int >( "oneReturn" ) );
}

TEST ( ScriptContextTest, testOneArgumentOneReturn )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );
    EXPECT_EQ ( 7, l.invoke< int >( "oneArgumentOneReturn", 7 ) );
}

TEST( ScriptContextTest, testMultiReturn )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

    int sum, difference;
    std::tie( sum, difference ) = l.invoke<int, int>( "sum_and_difference", 3, 1 );
    EXPECT_EQ( 4, sum );
    EXPECT_EQ( 2, difference );
}

TEST( ScriptContextTest, testHeterogeneousReturn )
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

TEST( ScriptContextTest, testCallCFunction )
{
    ScriptContext l;
    l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

    l.registerFunction( "cadd", std::function< int( int, int ) >( my_add ) );
    int answer = l.invoke< int >( "cadd", 3, 6 );

    EXPECT_EQ( 9, answer );
}

TEST( ScriptContextTest, testCallCFunctionFromLua )
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

TEST ( ScriptContextTest, testMultivalueCFunctionReturn )
{
	ScriptContext l;
	l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	l.registerFunction( "test_fun", &my_sum_and_difference );

	int sum, difference;
	std::tie( sum, difference ) = l.invoke< int, int >( "test_fun", -2, 2 );

	EXPECT_EQ( 0, sum );
	EXPECT_EQ( -4, difference );
}

TEST ( ScriptContextTest, testMultivalueCFuncFromLua )
{
	ScriptContext l;
	l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	l.registerFunction( "doozy_c", &my_sum_and_difference );

	EXPECT_EQ( false, l.isNil( "doozy_c" ) );

	int answer = l.invoke< int >( "doozy", 5 );

	EXPECT_EQ( -75, answer );
}

TEST ( ScriptContextTest, testCFuncDestructor )
{
	ScriptContext l;
	l.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	l.registerFunction( "doozy_c", &my_sum_and_difference );

	EXPECT_EQ( false, l.isNil( "doozy_c" ) );

	l.invoke< int >( "doozy", 5 );

	l.unregisterFunction( "doozy_c" );

	EXPECT_EQ( true, l.isNil( "doozy_c" ) );
}

TEST ( ScriptContextTest, testEval )
{
	ScriptContext context;
	context.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	EXPECT_EQ( 20, context.eval< float >( "player.position.x" ) );
	EXPECT_EQ( 90, context.eval< float >( "player.position.y" ) );
	EXPECT_EQ( "player", context.eval< std::string >( "player.name" ) );
	EXPECT_EQ( "warrior.obj", context.eval< std::string >( "player.model" ) );
}

TEST ( ScriptContextTest, testTest )
{
	ScriptContext context;
	context.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	EXPECT_TRUE( context.test( "player.position.x" ) );
	EXPECT_FALSE( context.test( "player.position.z" ) );
}

TEST ( ScriptContextTest, testDefaultValues )
{
	ScriptContext context;
	context.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	EXPECT_EQ( -1, context.eval< float >( "player.position.z" ) );
	EXPECT_EQ( "null", context.eval< std::string >( "player.background" ) );
}

TEST ( ScriptContextTest, testArrays )
{
	ScriptContext context;
	context.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	std::vector< int > values;

	EXPECT_EQ( 9, context.eval< int >( "#array" ) );
	EXPECT_EQ( 1, context.eval< int >( "array[ 1 ]" ) );
	EXPECT_EQ( 2, context.eval< int >( "array[ 2 ]" ) );
	EXPECT_EQ( 3, context.eval< int >( "array[ 3 ]" ) );
	EXPECT_EQ( 4, context.eval< int >( "array[ 4 ]" ) );
	EXPECT_EQ( 5, context.eval< int >( "array[ 5 ]" ) );
	EXPECT_EQ( 6, context.eval< int >( "array[ 6 ]" ) );
	EXPECT_EQ( 7, context.eval< int >( "array[ 7 ]" ) );
	EXPECT_EQ( 8, context.eval< int >( "array[ 8 ]" ) );
	EXPECT_EQ( 9, context.eval< int >( "array[ 9 ]" ) );
}

TEST ( ScriptContextTest, testArrayInTable )
{
	ScriptContext context;
	context.load( FileSystem::getInstance().pathForResource( "Scripts/simple.lua" ) );

	std::vector< int > values;

	EXPECT_EQ( 7, context.eval< int >( "#player.values" ) );
	EXPECT_EQ( 0, context.eval< int >( "player.values[ 1 ]" ) );
	EXPECT_EQ( 1, context.eval< int >( "player.values[ 2 ]" ) );
	EXPECT_EQ( 2, context.eval< int >( "player.values[ 3 ]" ) );
	EXPECT_EQ( 3, context.eval< int >( "player.values[ 4 ]" ) );
	EXPECT_EQ( 4, context.eval< int >( "player.values[ 5 ]" ) );
	EXPECT_EQ( 5, context.eval< int >( "player.values[ 6 ]" ) );
	EXPECT_EQ( 6, context.eval< int >( "player.values[ 7 ]" ) );
}

TEST ( ScriptContextTest, testNestedTables )
{
	ScriptContext context;
	context.load( FileSystem::getInstance().pathForResource( "Scripts/scene.lua" ) );

	int objCount = context.eval< int >( "#scene.objects" );
	EXPECT_EQ( 4, objCount );

	EXPECT_EQ( "obj1", context.eval< std::string >( "scene.objects[ 1 ].type" ) );
	EXPECT_EQ( "11", context.eval< std::string >( "scene.objects[ 1 ].position.x" ) );
	EXPECT_EQ( "21", context.eval< std::string >( "scene.objects[ 1 ].position.y" ) );

	EXPECT_EQ( "obj2", context.eval< std::string >( "scene.objects[ 2 ].type" ) );
	EXPECT_EQ( "12", context.eval< std::string >( "scene.objects[ 2 ].position.x" ) );
	EXPECT_EQ( "22", context.eval< std::string >( "scene.objects[ 2 ].position.y" ) );

	EXPECT_EQ( "obj3", context.eval< std::string >( "scene.objects[ 3 ].type" ) );
	EXPECT_EQ( "13", context.eval< std::string >( "scene.objects[ 3 ].position.x" ) );
	EXPECT_EQ( "23", context.eval< std::string >( "scene.objects[ 3 ].position.y" ) );

	EXPECT_EQ( "obj4", context.eval< std::string >( "scene.objects[ 4 ].type" ) );
	EXPECT_EQ( "14", context.eval< std::string >( "scene.objects[ 4 ].position.x" ) );
	EXPECT_EQ( "24", context.eval< std::string >( "scene.objects[ 4 ].position.y" ) );
}

TEST ( ScriptContextTest, testIterables )
{
	ScriptContext context;
	context.load( FileSystem::getInstance().pathForResource( "Scripts/scene.lua" ) );

	int i = 0;

	context.foreach( "scene.objects", [&i]( ScriptContext &c, ScriptContext::Iterable &it ) {
		i++;

		switch (it.getIndex()) {
			case 0:
				EXPECT_EQ( "obj1", it.eval< std::string >( "type" ) );
				EXPECT_EQ( "11", it.eval< std::string >( "position.x" ) );
				EXPECT_EQ( "21", it.eval< std::string >( "position.y" ) );
				break;

			case 1:
				EXPECT_EQ( "obj2", it.eval< std::string >( "type" ) );
				EXPECT_EQ( "12", it.eval< std::string >( "position.x" ) );
				EXPECT_EQ( "22", it.eval< std::string >( "position.y" ) );
				break;

			case 2:
				EXPECT_EQ( "obj3", it.eval< std::string >( "type" ) );
				EXPECT_EQ( "13", it.eval< std::string >( "position.x" ) );
				EXPECT_EQ( "23", it.eval< std::string >( "position.y" ) );
				break;

			case 3:
				EXPECT_EQ( "obj4", it.eval< std::string >( "type" ) );
				EXPECT_EQ( "14", it.eval< std::string >( "position.x" ) );
				EXPECT_EQ( "24", it.eval< std::string >( "position.y" ) );
				break;

			default:
				break;
		}
	});

	EXPECT_EQ( 4, i );
}


TEST ( ScriptContextTest, testMultipleTablesAndValues )
{
	ScriptContext context;
	context.load( FileSystem::getInstance().pathForResource( "Scripts/scene.lua" ) );
	
	EXPECT_EQ( "a scene", context.eval< std::string >( "scene.name" ) );

	EXPECT_EQ( 4, context.eval< int >( "#scene.objects" ) );

	EXPECT_EQ( "obj1", context.eval< std::string >( "scene.objects[ 1 ].type" ) );
	EXPECT_EQ( 11, context.eval< int >( "scene.objects[ 1 ].position.x" ) );
	EXPECT_EQ( 21, context.eval< int >( "scene.objects[ 1 ].position.y" ) );

	EXPECT_EQ( "obj2", context.eval< std::string >( "scene.objects[ 2 ].type" ) );
	EXPECT_EQ( 12, context.eval< int >( "scene.objects[ 2 ].position.x" ) );
	EXPECT_EQ( 22, context.eval< int >( "scene.objects[ 2 ].position.y" ) );

	EXPECT_EQ( "obj3", context.eval< std::string >( "scene.objects[ 3 ].type" ) );
	EXPECT_EQ( 13, context.eval< int >( "scene.objects[ 3 ].position.x" ) );
	EXPECT_EQ( 23, context.eval< int >( "scene.objects[ 3 ].position.y" ) );

	EXPECT_EQ( "obj4", context.eval< std::string >( "scene.objects[ 4 ].type" ) );
	EXPECT_EQ( 14, context.eval< int >( "scene.objects[ 4 ].position.x" ) );
	EXPECT_EQ( 24, context.eval< int >( "scene.objects[ 4 ].position.y" ) );

	EXPECT_EQ( "bridge", context.eval< std::string >( "scene.map" ) );

	EXPECT_EQ( 1024, context.eval< int >( "settings.resolution.width" ) );
	EXPECT_EQ( 768, context.eval< int >( "settings.resolution.height" ) );
}

