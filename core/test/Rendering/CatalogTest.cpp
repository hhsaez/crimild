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
	auto catalog = crimild::alloc< Catalog< Texture >>();
}

TEST( CatalogTest, destruction )
{
	auto texture = crimild::alloc< Texture >();

	{
		auto catalog = crimild::alloc< Catalog< Texture >>();
        catalog->bind( crimild::get_ptr( texture ) );

		EXPECT_TRUE( catalog->hasResources() );
		EXPECT_EQ( 0, texture->getCatalogId() );
		EXPECT_EQ( catalog.get(), texture->getCatalog() );
	}

	EXPECT_EQ( -1, texture->getCatalogId() );
	EXPECT_EQ( nullptr, texture->getCatalog() );

}

TEST( CatalogTest, bindResource )
{
	auto catalog = crimild::alloc< Catalog< Texture >>();
	auto texture = crimild::alloc< Texture >();

    catalog->bind( crimild::get_ptr( texture ) );

	EXPECT_TRUE( catalog->hasResources() );
	EXPECT_EQ( 0, texture->getCatalogId() );
	EXPECT_EQ( catalog.get(), texture->getCatalog() );
}

TEST( CatalogTest, bindResourceTwice )
{
	auto catalog = crimild::alloc< Catalog< Texture >>();
	auto texture = crimild::alloc< Texture >();

    catalog->bind( crimild::get_ptr( texture ) );

	EXPECT_TRUE( catalog->hasResources() );
	EXPECT_EQ( 0, texture->getCatalogId() );
	EXPECT_EQ( catalog.get(), texture->getCatalog() );

	catalog->bind( crimild::get_ptr( texture ) );

	EXPECT_TRUE( catalog->hasResources() );
	EXPECT_EQ( 0, texture->getCatalogId() );
	EXPECT_EQ( catalog.get(), texture->getCatalog() );
}

TEST( CatalogTest, unbindResource )
{
	auto catalog = crimild::alloc< Catalog< Texture >>();
	auto texture = crimild::alloc< Texture >();

	catalog->bind( crimild::get_ptr( texture ) );

	EXPECT_TRUE( catalog->hasResources() );
	EXPECT_EQ( 0, texture->getCatalogId() );
	EXPECT_EQ( catalog.get(), texture->getCatalog() );

	catalog->unbind( crimild::get_ptr( texture ) );

	EXPECT_TRUE( catalog->hasResources() );
	EXPECT_EQ( 0, texture->getCatalogId() );
	EXPECT_EQ( catalog.get(), texture->getCatalog() );
}

TEST( CatalogTest, loadResource )
{
	auto catalog = crimild::alloc< Catalog< Texture >>();
	auto texture = crimild::alloc< Texture >();

	catalog->load( crimild::get_ptr( texture ) );

	EXPECT_TRUE( catalog->hasResources() );
	EXPECT_EQ( 0, texture->getCatalogId() );
	EXPECT_EQ( catalog.get(), texture->getCatalog() );
}

TEST( CatalogTest, unloadResource )
{
	auto catalog = crimild::alloc< Catalog< Texture >>();
	auto texture = crimild::alloc< Texture >();

	catalog->load( crimild::get_ptr( texture ) );

	EXPECT_TRUE( catalog->hasResources() );
	EXPECT_EQ( 0, texture->getCatalogId() );
	EXPECT_EQ( catalog.get(), texture->getCatalog() );

	catalog->unload( crimild::get_ptr( texture ) );

	EXPECT_FALSE( catalog->hasResources() );
	EXPECT_EQ( -1, texture->getCatalogId() );
	EXPECT_EQ( nullptr, texture->getCatalog() );
}

TEST( CatalogTest, automaticResourceUnload )
{
	auto catalog = crimild::alloc< Catalog< Texture >>();

	{
		auto texture = crimild::alloc< Texture >();
		catalog->bind( crimild::get_ptr( texture ) );

		EXPECT_TRUE( catalog->hasResources() );
		EXPECT_EQ( 0, texture->getCatalogId() );
		EXPECT_EQ( catalog.get(), texture->getCatalog() );
	}

	EXPECT_FALSE( catalog->hasResources() );
}

