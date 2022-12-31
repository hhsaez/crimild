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

#include "Editor/EditorProject.hpp"

#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"

#include <gtest/gtest.h>

using namespace crimild;
using namespace crimild::editor;

TEST( Project, with_default_values )
{
    const Project project;
    EXPECT_STREQ( "Crimild", project.getName().c_str() );
    EXPECT_EQ( Version( 1, 0, 0 ), project.getVersion() );
    EXPECT_EQ( std::string( "main" ), project.getCurrentSceneName() );
}

TEST( Project, with_custom_values )
{
    Project project( "MyProject", Version( 1, 2, 3 ) );
    project.setCurrentSceneName( "test" );

    EXPECT_STREQ( "MyProject", project.getName().c_str() );
    EXPECT_EQ( Version( 1, 2, 3 ), project.getVersion() );
    EXPECT_EQ( std::string( "test" ), project.getCurrentSceneName() );
}

TEST( Project, path_handling )
{
    Project project( "MyProject", Version( 1, 2, 3 ) );
    project.setFilePath( "/path/to/MyProject/project.crimild" );

    const auto expected = std::filesystem::absolute( "/path/to/MyProject/project.crimild" );
    EXPECT_EQ( project.getFilePath(), expected );
    EXPECT_EQ( project.getRootDirectory(), std::filesystem::absolute( "/path/to/MyProject" ) );
    EXPECT_EQ( project.getAssetsDirectory(), std::filesystem::absolute( "/path/to/MyProject/Assets" ) );
    EXPECT_EQ( project.getScenesDirectory(), std::filesystem::absolute( "/path/to/MyProject/Assets/Scenes" ) );

    EXPECT_EQ( project.getCurrentSceneName(), std::string( "main" ) );
    EXPECT_EQ( project.getScenePath( project.getCurrentSceneName() ), std::filesystem::absolute( "/path/to/MyProject/Assets/Scenes/main.crimild" ) );

    project.setCurrentSceneName( "test" );
    EXPECT_EQ( project.getScenePath( project.getCurrentSceneName() ), std::filesystem::absolute( "/path/to/MyProject/Assets/Scenes/test.crimild" ) );
}

TEST( Project, coding )
{
    auto project = crimild::alloc< Project >( "MyProject", Version( 4, 5, 6 ) );
    project->setCurrentSceneName( "test" );

    coding::MemoryEncoder encoder;
    encoder.encode( project );
    const auto encoded = encoder.getBytes();

    coding::MemoryDecoder decoder;
    ASSERT_TRUE( decoder.fromBytes( encoded ) );
    const auto decoded = decoder.getObjectAt< Project >( 0 );
    ASSERT_NE( nullptr, decoded );

    EXPECT_STREQ( "MyProject", decoded->getName().c_str() );
    EXPECT_EQ( Version( 4, 5, 6 ), decoded->getVersion() );
    EXPECT_EQ( std::string( "test" ), decoded->getCurrentSceneName() );
}
