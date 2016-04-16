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

#include "Text.hpp"

#include "Primitives/Primitive.hpp"

#include "Rendering/ImageTGA.hpp"
#include "Rendering/Renderer.hpp"

#include "Foundation/Log.hpp"

#include "Components/MaterialComponent.hpp"

#include "Simulation/AssetManager.hpp"
#include "Simulation/FileSystem.hpp"

#include <fstream>

using namespace crimild;

Font::Font( std::string fontDefFile )
{
	loadGlyphs( fontDefFile );
}

Font::~Font( void )
{

}

Texture *Font::getTexture( void )
{
	return AssetManager::getInstance()->get< Texture >( _textureFileName );
}

Texture *Font::getSDFTexture( void )
{
	return AssetManager::getInstance()->get< Texture >( _sdfTextureFileName );
}

void Font::loadGlyphs( std::string file )
{
	_glyphs.clear();

	auto fontNamePrefix = file.substr( 0, file.find_last_of( "." ) );
	fontNamePrefix = FileSystem::getInstance().getRelativePath( fontNamePrefix );
	_textureFileName = fontNamePrefix + ".tga";
	_sdfTextureFileName = fontNamePrefix + "_sdf.tga";

	std::ifstream input;
	input.open( file );
	if ( !input.is_open() ) {
		crimild::Log::Error << "Cannot open glyph file " << file << crimild::Log::End;
		return;
	}

	char buffer[ 1024 ];
	while ( !input.eof() ) {
		input.getline( buffer, 1024 );
		std::stringstream line;
		line << buffer;

		Font::Glyph glyph;
		int symbol;
		line >> symbol
			 >> glyph.width
			 >> glyph.height
			 >> glyph.bearingX
			 >> glyph.bearingY
			 >> glyph.advance
			 >> glyph.uOffset
			 >> glyph.vOffset
			 >> glyph.u
			 >> glyph.v;
		glyph.symbol = ( char ) symbol;
		_glyphs[ glyph.symbol ] = glyph;
	}
}

Text::Text( void )
    : _primitive( crimild::alloc< Primitive >( Primitive::Type::TRIANGLES ) ),
      _material( crimild::alloc< Material >() )
{
	_text = "";
	_size = 1.0f;
	attachPrimitive( _primitive );
	getComponent< MaterialComponent >()->attachMaterial( _material );
}

Text::~Text( void )
{

}

void Text::accept( NodeVisitor &visitor )
{
    visitor.visitText( this );
}

void Text::setText( std::string text )
{
	_text = text;
	if ( _text == "" ) {
		_text = " ";
	}
	updatePrimitive();
}

void Text::setSize( float size )
{
	_size = size;
	updatePrimitive();
}

void Text::setFont( SharedPointer< Font > const &font )
{
	if ( _font == font ) {
		return;
	}

	_font = font;

	auto sdfProgram = AssetManager::getInstance()->get< ShaderProgram >( Renderer::SHADER_PROGRAM_TEXT_SDF );
	if ( sdfProgram != nullptr ) {
		// SDF Supported
		_material->setColorMap( _font->getSDFTexture() );
		_material->setProgram( sdfProgram );
	}
	else {
		// SDF not supported by renderer
		_material->setColorMap( _font->getTexture() );
		_material->setProgram( AssetManager::getInstance()->get< ShaderProgram >( Renderer::SHADER_PROGRAM_TEXT_BASIC ) );
	}

	_material->getAlphaState()->setEnabled( true );
    
	updatePrimitive();
}

void Text::updatePrimitive( void )
{
	std::vector< VertexPrecision > vertices;
	std::vector< IndexPrecision > indices;

	float horiAdvance = 0.0f;
	float vertAdvance = 0.0f;
	for ( int i = 0; i < _text.length(); i++ ) {
		if ( _text[ i ] == '\n' ) {
			vertAdvance -= _size;
			horiAdvance = 0.0f;
			continue;
		}

		Font::Glyph glyph = _font->getGlyph( _text[ i ] );

		float minX = _size * ( horiAdvance + glyph.bearingX );
		float maxX = minX + _size * glyph.width;
		float minY = vertAdvance + _size * ( glyph.bearingY - glyph.height );
		float maxY = minY + _size * glyph.height;
		float s0 = glyph.uOffset;
		float s1 = s0 + glyph.u;
		float t0 = glyph.vOffset;
		float t1 = t0 + glyph.v;

		vertices.push_back( minX ); 
		vertices.push_back( maxY );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 1.0f );
		vertices.push_back( s0 );
		vertices.push_back( 1.0f - t0 );
		indices.push_back( indices.size() );

		vertices.push_back( minX ); 
		vertices.push_back( minY );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 1.0f );
		vertices.push_back( s0 );
		vertices.push_back( 1.0f - t1 );
		indices.push_back( indices.size() );

		vertices.push_back( maxX ); 
		vertices.push_back( minY );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 1.0f );
		vertices.push_back( s1 );
		vertices.push_back( 1.0f - t1 );
		indices.push_back( indices.size() );

		vertices.push_back( minX ); 
		vertices.push_back( maxY );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 1.0f );
		vertices.push_back( s0 );
		vertices.push_back( 1.0f - t0 );
		indices.push_back( indices.size() );

		vertices.push_back( maxX ); 
		vertices.push_back( minY );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 1.0f );
		vertices.push_back( s1 );
		vertices.push_back( 1.0f - t1 );
		indices.push_back( indices.size() );

		vertices.push_back( maxX ); 
		vertices.push_back( maxY );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 0.0f );
		vertices.push_back( 1.0f );
		vertices.push_back( s1 );
		vertices.push_back( 1.0f - t0 );
		indices.push_back( indices.size() );

		horiAdvance += glyph.advance;
	}

    auto format = VertexFormat::VF_P3_N3_UV2;
    auto vbo = crimild::alloc< VertexBufferObject >( format, vertices.size() / format.getVertexSize(), &vertices[ 0 ] );
    _primitive->setVertexBuffer( vbo );
    
    auto ibo = crimild::alloc< IndexBufferObject >( indices.size(), &indices[ 0 ] );
	_primitive->setIndexBuffer( ibo );
	
	updateModelBounds();
}

