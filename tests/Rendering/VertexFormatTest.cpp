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

#include <Crimild.hpp>

#include "gtest/gtest.h"

using namespace Crimild;

TEST( VertexFormatTest, construction )
{
	VertexFormat vf( 3, 0, 0 );
	
	EXPECT_TRUE( vf.hasPositions() );
	EXPECT_EQ( 3, vf.getPositionComponents() );
	EXPECT_EQ( 0, vf.getPositionsOffset() );

	EXPECT_FALSE( vf.hasNormals() );
	EXPECT_EQ( 0, vf.getNormalComponents() );
	EXPECT_EQ( 3, vf.getNormalsOffset() );
	
	EXPECT_FALSE( vf.hasTextureCoords() );
	EXPECT_EQ( 0, vf.getTextureCoordComponents() );
	EXPECT_EQ( 3, vf.getTextureCoordsOffset() );

	EXPECT_EQ( 3, vf.getVertexSize() );
	EXPECT_EQ( 3 * sizeof( float ), vf.getVertexSizeInBytes() );
	
	vf = VertexFormat( 3, 3, 2 );
	
	EXPECT_TRUE( vf.hasPositions() );
	EXPECT_EQ( 3, vf.getPositionComponents() );
	EXPECT_EQ( 0, vf.getPositionsOffset() );

	EXPECT_TRUE( vf.hasNormals() );
	EXPECT_EQ( 3, vf.getNormalComponents() );
	EXPECT_EQ( 3, vf.getNormalsOffset() );
	
	EXPECT_TRUE( vf.hasTextureCoords() );
	EXPECT_EQ( 2, vf.getTextureCoordComponents() );
	EXPECT_EQ( 6, vf.getTextureCoordsOffset() );

	EXPECT_EQ( 8, vf.getVertexSize() );
	EXPECT_EQ( 8 * sizeof( float ), vf.getVertexSizeInBytes() );
	
}

