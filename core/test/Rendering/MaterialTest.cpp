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

#include "Rendering/Material.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( MaterialTest, construction )
{
	auto material = crimild::alloc< Material >();

	EXPECT_EQ( RGBAColorf( 0.0f, 0.0f, 0.0f, 1.0f ), material->getAmbient() );
	EXPECT_EQ( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ), material->getDiffuse() );
	EXPECT_EQ( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ), material->getSpecular() );
	EXPECT_EQ( 50.0f, material->getShininess() );

	EXPECT_EQ( nullptr, material->getColorMap() );
	EXPECT_EQ( nullptr, material->getProgram() );
}

TEST( MaterialTest, setProgram )
{
	auto material = crimild::alloc< Material >();

	auto vs = crimild::alloc< VertexShader >( "vs code" );
	auto fs = crimild::alloc< FragmentShader >( "fs code" );
	auto program = crimild::alloc< ShaderProgram >( vs, fs );
	material->setProgram( program );

	ASSERT_EQ( crimild::get_ptr( program ), material->getProgram() );
}

TEST( MaterialTest, setColorMap )
{
	auto material = crimild::alloc< Material >();

	auto image = crimild::alloc< Image >( 0, 0, 0, nullptr );
	auto texture = crimild::alloc< Texture >( image );
	material->setColorMap( texture );

	ASSERT_EQ( crimild::get_ptr( texture ), material->getColorMap() );
}

