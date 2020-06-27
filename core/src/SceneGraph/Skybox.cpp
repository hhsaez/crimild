/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "Skybox.hpp"

#include "Components/RenderStateComponent.hpp"
#include "Rendering/DepthState.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/ImageTGA.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/ShaderProgramLibrary.hpp"
#include "Rendering/Uniforms/ModelViewProjectionUniformBuffer.hpp"
#include "Simulation/FileSystem.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"

using namespace crimild;

Skybox::Skybox( ImageArray const &faces ) noexcept
{
    configure( faces );
}

void Skybox::configure( Skybox::ImageArray const &images ) noexcept
{
#if 0
	m_faces = images;

    m_texture = crimild::alloc< Texture >( images );

    auto const SKYBOX_SIZE = 10.0f;

    auto vbo = crimild::alloc< VertexP3Buffer >(
        containers::Array< VertexP3 > {
            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },

            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },

            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },

            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },

            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE ) },

            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE ) },
            { .position = Vector3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },
            { .position = Vector3f( SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE ) },
        }
    );

    auto ibo = crimild::alloc< IndexUInt32Buffer >(
        containers::Array< crimild::UInt32 > {
        	0, 1, 2,
        	3, 4, 5,
        	6, 7, 8,
            9, 10, 11,
            12, 13, 14,
            15, 16, 17,
            18, 19, 20,
            21, 22, 23,
            24, 25, 26,
            27, 28, 29,
            30, 31, 32,
            33, 34, 35,
        }
    );

    auto program = ShaderProgramLibrary::getInstance()->get( constants::SHADER_PROGRAM_UNLIT_SKYBOX_P3 );
    auto pipeline = [&] {
        auto pipeline = crimild::alloc< Pipeline >();
        pipeline->program = crimild::retain( program );
        pipeline->depthState = DepthState::DISABLED;
        pipeline->cullFaceState = CullFaceState::DISABLED;
        return pipeline;
    }();

    attachComponent( [&] {
        auto renderable = crimild::alloc< RenderStateComponent >();
        renderable->pipeline = pipeline;
        renderable->vbo = vbo;
        renderable->ibo = ibo;
        renderable->uniforms = {
            [&] {
                auto ubo = crimild::alloc< ModelViewProjectionUniformBuffer >();
                ubo->node = this;
                return ubo;
            }(),
        };
        renderable->textures = { m_texture };
        return renderable;
    }());

	setLayer( Node::Layer::SKYBOX );
	setCullMode( Node::CullMode::NEVER );
#endif
}

void Skybox::encode( coding::Encoder &encoder )
{
	Geometry::encode( encoder );
}

void Skybox::decode( coding::Decoder &decoder )
{
	Geometry::decode( decoder );

    Array< std::string > images;
	decoder.decode( "images", images );

	ImageArray faces;
	images.each( [ &faces ]( std::string fileName ) {
        faces.add( crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( fileName ) ) );
	});
	configure( faces );
}
