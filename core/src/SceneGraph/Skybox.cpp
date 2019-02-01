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

#include "Rendering/IndexBufferObject.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/ImageTGA.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Programs/SkyboxShaderProgram.hpp"
#include "Primitives/Primitive.hpp"
#include "Components/MaterialComponent.hpp"
#include "Simulation/FileSystem.hpp"

using namespace crimild;

Skybox::Skybox( void )
{
	
}

Skybox::Skybox( ImageArray const &faces )
	: _faces( faces )
{

}

Skybox::~Skybox( void )
{
	
}

void Skybox::configure( Skybox::ImageArray const &images )
{
	_faces = images;
	
	float vertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		
		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	
	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, 36, vertices );
	auto ibo = crimild::alloc< IndexBufferObject >( 36, nullptr );
	ibo->generateIncrementalIndices();
	
	auto p = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
	p->setVertexBuffer( vbo );
	p->setIndexBuffer( ibo );
	attachPrimitive( p );
	
	auto m = crimild::alloc< Material >();
	m->setDiffuse( RGBAColorf::ONE );
	
	auto texture = crimild::alloc< Texture >( _faces );
	texture->setWrapMode( Texture::WrapMode::CLAMP_TO_EDGE );
	m->setColorMap( texture );

	m->setProgram( crimild::alloc< SkyboxShaderProgram >() );
	
	getComponent< MaterialComponent >()->attachMaterial( m );
	
	setLayer( Node::Layer::SKYBOX );
	setCullMode( Node::CullMode::NEVER );
}

void Skybox::encode( coding::Encoder &encoder )
{
	Geometry::encode( encoder );
}

void Skybox::decode( coding::Decoder &decoder )
{
	Geometry::decode( decoder );

    containers::Array< std::string > images;
	decoder.decode( "images", images );

	ImageArray faces;
	images.each( [ &faces ]( std::string fileName ) {
        faces.add( crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( fileName ) ) );
	});
	configure( faces );
}

