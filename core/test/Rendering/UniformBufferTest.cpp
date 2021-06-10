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

#include "Rendering/UniformBuffer.hpp"

#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "Rendering/BufferView.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( UniformBuffer, withSingleValue )
{
    auto uniform = crimild::alloc< UniformBuffer >( Vector3f::Constants::ZERO );

    ASSERT_NE( nullptr, uniform->getBufferView() );
    ASSERT_EQ( 3 * sizeof( crimild::Real32 ), uniform->getBufferView()->getLength() );

    ASSERT_EQ( Vector3f::Constants::ZERO, uniform->getValue< Vector3f >() );

    uniform->setValue( Vector3f { 1.0f, 2.0f, 3.0f } );

    ASSERT_EQ( ( Vector3f { 1.0f, 2.0f, 3.0f } ), uniform->getValue< Vector3f >() );
}

TEST( UniformBuffer, withStruct )
{
    struct Data {
        Matrix4f proj;
        Matrix4f view;
        ColorRGBA color;
        float weights;
        Vector2i indices;
    };

    auto uniform = crimild::alloc< UniformBuffer >( Data {} );

    ASSERT_NE( nullptr, uniform->getBufferView() );
    ASSERT_EQ( sizeof( Data ), uniform->getBufferView()->getLength() );

    uniform->getValue< Data >().color = ColorRGBA { 1.0f, 0.0f, 1.0f, 0.5f };

    ASSERT_EQ( ( ColorRGBA { 1.0f, 0.0f, 1.0f, 0.5f } ), uniform->getValue< Data >().color );
}
