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

#include "Rendering/VulkanShaderLibrary.hpp"
#include "Rendering/Shader.hpp"

using namespace crimild;

template<>
RenderResourceLibrary< Shader >::RenderResourceLibrary( void ) noexcept
{
    auto createShader = []( Shader::Stage stage, const unsigned char *rawData, crimild::Size size ) {
        std::vector< char > data( size + ( size % 4 ) );
        memcpy( &data[ 0 ], rawData, size );
        return crimild::alloc< Shader >( stage, data );
    };

    add(
        constants::SHADER_UNLIT_VERTEX_P2C3,
        [&] {
			#include "Shaders/unlit/vert_P2C3.inl"
            return createShader( Shader::Stage::VERTEX, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_UNLIT_VERTEX_P2C3TC2,
        [&] {
			#include "Shaders/unlit/vert_P2C3TC2.inl"
           	return createShader( Shader::Stage::VERTEX, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_UNLIT_FRAGMENT_COLOR,
        [&] {
           #include "Shaders/unlit/frag_color.inl"
           return createShader( Shader::Stage::FRAGMENT, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_UNLIT_FRAGMENT_TEXTURE_COLOR,
        [&] {
           #include "Shaders/unlit/frag_texture_color.inl"
           return createShader( Shader::Stage::FRAGMENT, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_UNLIT_TEXTURE_P3N3TC2_VERT,
        [&] {
           #include "Shaders/unlit/texture_P3N3TC2.vert.inl"
           return createShader( Shader::Stage::VERTEX, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_UNLIT_TEXTURE_P3N3TC2_FRAG,
        [&] {
           #include "Shaders/unlit/texture_P3N3TC2.frag.inl"
           return createShader( Shader::Stage::FRAGMENT, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_UNLIT_SKYBOX_P3_VERT,
        [&] {
           #include "Shaders/unlit/skybox_P3.vert.inl"
           return createShader( Shader::Stage::VERTEX, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_UNLIT_SKYBOX_P3_FRAG,
        [&] {
           #include "Shaders/unlit/skybox_P3.frag.inl"
           return createShader( Shader::Stage::FRAGMENT, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_DEBUG_POSITION_P3_VERT,
        [&] {
           #include "Shaders/debug/position_P3.vert.inl"
           return createShader( Shader::Stage::VERTEX, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_DEBUG_POSITION_P3_FRAG,
        [&] {
           #include "Shaders/debug/position_P3.frag.inl"
           return createShader( Shader::Stage::FRAGMENT, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_DEBUG_POSITION_P3N3TC2_VERT,
        [&] {
           #include "Shaders/debug/position_P3N3TC2.vert.inl"
           return createShader( Shader::Stage::VERTEX, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );

    add(
        constants::SHADER_DEBUG_POSITION_P3N3TC2_FRAG,
        [&] {
           #include "Shaders/debug/position_P3N3TC2.frag.inl"
           return createShader( Shader::Stage::FRAGMENT, RESOURCE_BYTES, sizeof( RESOURCE_BYTES ) );
        }
    );
}

