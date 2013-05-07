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

#include "Rendering/ShaderProgram.hpp"

#include "gtest/gtest.h"

using namespace Crimild;

TEST( ShaderProgramTest, construction )
{
	VertexShaderPtr vs( new VertexShader( "vs code" ) );
	FragmentShaderPtr fs( new FragmentShader( "fs code" ) );
	ShaderProgramPtr program( new ShaderProgram( vs, fs ) );

	EXPECT_EQ( vs.get(), program->getVertexShader() );
	EXPECT_EQ( fs.get(), program->getFragmentShader() );
}

TEST( ShaderProgramTest, locations )
{
	VertexShaderPtr vs( new VertexShader( "vs code" ) );
	FragmentShaderPtr fs( new FragmentShader( "fs code" ) );
	ShaderProgramPtr program( new ShaderProgram( vs, fs ) );

	ShaderLocationPtr vertexWeightLocation( new ShaderLocation( ShaderLocation::Type::ATTRIBUTE, "vertexWeight" ) );
	program->registerLocation( vertexWeightLocation );
	ASSERT_EQ( vertexWeightLocation.get(), program->getLocation( vertexWeightLocation->getName() ) );
	EXPECT_FALSE( vertexWeightLocation->isValid() );

	ShaderLocationPtr timeLocation( new ShaderLocation( ShaderLocation::Type::UNIFORM, "time" ) );
	program->registerLocation( timeLocation );
	ASSERT_EQ( timeLocation.get(), program->getLocation( timeLocation->getName() ) );
	EXPECT_FALSE( timeLocation->isValid() );

	int i = 0; 
	program->foreachLocation( [&]( ShaderLocationPtr &loc ) mutable {
		i++;
	});
	EXPECT_EQ( 2, i );

	vertexWeightLocation->setLocation( 2 );
	timeLocation->setLocation( 2 );

	EXPECT_TRUE( vertexWeightLocation->isValid() );
	EXPECT_TRUE( vertexWeightLocation->isValid() );

	program->resetLocations();

	EXPECT_FALSE( vertexWeightLocation->isValid() );
	EXPECT_FALSE( vertexWeightLocation->isValid() );
}

TEST( ShaderProgramTest, multipleLightLocations )
{
	ShaderProgramPtr program( new ShaderProgram( nullptr, nullptr ) );

	program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + 0, "uLights[0].position" );
	program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM + 0, "uLights[0].attenuation" );

	program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + 1, "uLights[1].position" );
	program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM + 1, "uLights[1].attenuation" );

	program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + 2, "uLights[2].position" );
	program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM + 2, "uLights[2].attenuation" );

	program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + 50, "uLights[50].position" );
	program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM + 50, "uLights[50].attenuation" );

	int i = 0;
	program->foreachLocation( [&]( ShaderLocationPtr &location ) mutable {
		location->setLocation( i++ );
	});

	EXPECT_EQ( 1, program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + 0 )->getLocation() );
	EXPECT_EQ( 3, program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + 1 )->getLocation() );
	EXPECT_EQ( 5, program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + 2 )->getLocation() );
	EXPECT_EQ( 7, program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + 50 )->getLocation() );
}

