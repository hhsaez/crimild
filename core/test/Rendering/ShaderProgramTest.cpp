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

#include "Rendering/ShaderProgram.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( ShaderProgram, defaultConstruction )
{
    auto program = crimild::alloc< ShaderProgram >();

    EXPECT_TRUE( program->getShaders().empty() );
}

TEST( ShaderProgram, constructionWithShaders )
{
    auto program = crimild::alloc< ShaderProgram >(
        ShaderProgram::ShaderArray {
            crimild::alloc< Shader >( Shader::Stage::VERTEX ),
            crimild::alloc< Shader >( Shader::Stage::GEOMETRY ),
            crimild::alloc< Shader >( Shader::Stage::FRAGMENT ),
        } );

    EXPECT_EQ( 3, program->getShaders().size() );
    EXPECT_EQ( Shader::Stage::VERTEX, program->getShaders()[ 0 ]->getStage() );
    EXPECT_EQ( Shader::Stage::GEOMETRY, program->getShaders()[ 1 ]->getStage() );
    EXPECT_EQ( Shader::Stage::FRAGMENT, program->getShaders()[ 2 ]->getStage() );
}
