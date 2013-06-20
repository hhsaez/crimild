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

#include "Rendering/Catalog.hpp"
#include "Rendering/Texture.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( CatalogTest, construction )
{
	Catalog< Texture > catalog;
}

TEST( CatalogTest, destruction )
{
	TexturePtr texture( new Texture( nullptr ) );

	{
		Catalog< Texture > catalog;
		catalog.bind( texture.get() );

		EXPECT_TRUE( catalog.hasResources() );
		EXPECT_EQ( 1, texture->getCatalogId() );
		EXPECT_EQ( &catalog, texture->getCatalog() );
	}

	EXPECT_EQ( -1, texture->getCatalogId() );
	EXPECT_EQ( nullptr, texture->getCatalog() );

}

TEST( CatalogTest, bindResource )
{
	Catalog< Texture > catalog;
	TexturePtr texture( new Texture( nullptr ) );
	catalog.bind( texture.get() );

	EXPECT_TRUE( catalog.hasResources() );
	EXPECT_EQ( 1, texture->getCatalogId() );
	EXPECT_EQ( &catalog, texture->getCatalog() );
}

TEST( CatalogTest, bindResourceTwice )
{
	Catalog< Texture > catalog;
	TexturePtr texture( new Texture( nullptr ) );
	
	catalog.bind( texture.get() );

	EXPECT_TRUE( catalog.hasResources() );
	EXPECT_EQ( 1, texture->getCatalogId() );
	EXPECT_EQ( &catalog, texture->getCatalog() );

	catalog.bind( texture.get() );

	EXPECT_TRUE( catalog.hasResources() );
	EXPECT_EQ( 1, texture->getCatalogId() );
	EXPECT_EQ( &catalog, texture->getCatalog() );
}

TEST( CatalogTest, unbindResource )
{
	Catalog< Texture > catalog;
	TexturePtr texture( new Texture( nullptr ) );
	
	catalog.bind( texture.get() );

	EXPECT_TRUE( catalog.hasResources() );
	EXPECT_EQ( 1, texture->getCatalogId() );
	EXPECT_EQ( &catalog, texture->getCatalog() );

	catalog.unbind( texture.get() );

	EXPECT_TRUE( catalog.hasResources() );
	EXPECT_EQ( 1, texture->getCatalogId() );
	EXPECT_EQ( &catalog, texture->getCatalog() );
}

TEST( CatalogTest, loadResource )
{
	Catalog< Texture > catalog;
	TexturePtr texture( new Texture( nullptr ) );
	
	catalog.load( texture.get() );

	EXPECT_TRUE( catalog.hasResources() );
	EXPECT_EQ( 1, texture->getCatalogId() );
	EXPECT_EQ( &catalog, texture->getCatalog() );
}

TEST( CatalogTest, unloadResource )
{
	Catalog< Texture > catalog;
	TexturePtr texture( new Texture( nullptr ) );
	
	catalog.load( texture.get() );

	EXPECT_TRUE( catalog.hasResources() );
	EXPECT_EQ( 1, texture->getCatalogId() );
	EXPECT_EQ( &catalog, texture->getCatalog() );

	catalog.unload( texture.get() );

	EXPECT_FALSE( catalog.hasResources() );
	EXPECT_EQ( -1, texture->getCatalogId() );
	EXPECT_EQ( nullptr, texture->getCatalog() );
}

TEST( CatalogTest, automaticResourceUnload )
{
	Catalog< Texture > catalog;

	{
		TexturePtr texture( new Texture( nullptr ) );
		catalog.bind( texture.get() );

		EXPECT_TRUE( catalog.hasResources() );
		EXPECT_EQ( 1, texture->getCatalogId() );
		EXPECT_EQ( &catalog, texture->getCatalog() );
	}

	EXPECT_FALSE( catalog.hasResources() );
}

