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

#include "Components/MaterialComponent.hpp"

#include "Rendering/Materials/UnlitMaterial.hpp"

#include <Crimild_Coding.hpp>
#include <Crimild_Mathematics.hpp>
#include <gtest/gtest.h>

using namespace crimild;

TEST( MaterialComponent, attachMaterial )
{
    auto materials = crimild::alloc< MaterialComponent >();

    EXPECT_FALSE( materials->hasMaterials() );

    auto material = crimild::alloc< Material >();
    materials->attachMaterial( material );

    EXPECT_TRUE( materials->hasMaterials() );

    int i = 0;
    materials->forEachMaterial( [ &i, material ]( Material *m ) {
        i++;
        EXPECT_EQ( m, crimild::get_ptr( material ) );
    } );
    EXPECT_EQ( 1, i );
}

TEST( MaterialComponent, coding )
{
    auto material = crimild::alloc< UnlitMaterial >();
    material->setColor( ColorRGBA { 0.7f, 0.7f, 0.7f, 1.0f } );
    material->setTexture( crimild::alloc< Texture >() );

    auto materials = crimild::alloc< MaterialComponent >();
    materials->attachMaterial( material );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    encoder->encode( materials );
    auto bytes = encoder->getBytes();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();
    decoder->fromBytes( bytes );

    ASSERT_NE( 0, decoder->getObjectCount() );

    auto ms = decoder->getObjectAt< MaterialComponent >( 0 );
    ASSERT_TRUE( ms != nullptr );

    ASSERT_TRUE( ms->hasMaterials() );

    ASSERT_NE( nullptr, ms->first() );
    EXPECT_TRUE( isEqual( ColorRGBA { 0.7f, 0.7f, 0.7f, 1.0f }, static_cast< UnlitMaterial * >( ms->first() )->getColor() ) );
}
