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

#include "Material.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::Material )

using namespace crimild;

Material::Material( void )
	: _ambient( 0.0f, 0.0f, 0.0f, 1.0f ),
	  _diffuse( 1.0f, 1.0f, 1.0f, 1.0f ),
	  _specular( 1.0f, 1.0f, 1.0f, 1.0f ),
	  _shininess( 50.0f ),
	  _emissive( 0.0f ),
      _depthState( crimild::alloc< DepthState >( true ) ),
      _alphaState( crimild::alloc< AlphaState >( false ) ),
      _cullFaceState( crimild::alloc< CullFaceState >( true, CullFaceState::CullFaceMode::BACK ) ),
      _colorMaskState( crimild::alloc< ColorMaskState >( true, true, true, true, true ) )
{

}

Material::~Material( void )
{

}

void Material::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

	encoder.encode( "ambient", _ambient );
	encoder.encode( "diffuse", _diffuse );
	encoder.encode( "specular", _specular );
	encoder.encode( "emissive", _emissive );
	encoder.encode( "shininess", _shininess );
	encoder.encode( "colorMap", _colorMap );
	encoder.encode( "normalMap", _normalMap );
	encoder.encode( "specularMap", _specularMap );
	encoder.encode( "emissiveMap", _emissiveMap );
}

void Material::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

	decoder.decode( "ambient", _ambient );
	decoder.decode( "diffuse", _diffuse );
	decoder.decode( "specular", _specular );
	decoder.decode( "emissive", _emissive );
	decoder.decode( "shininess", _shininess );
    decoder.decode( "colorMap", _colorMap );
	decoder.decode( "normalMap", _normalMap );
	decoder.decode( "specularMap", _specularMap );
	decoder.decode( "emissiveMap", _emissiveMap );
}

bool Material::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

	if ( getColorMap() != nullptr ) {
		getColorMap()->registerInStream( s );
	}

	if ( getNormalMap() != nullptr ) {
		getNormalMap()->registerInStream( s );
	}

	if ( getSpecularMap() != nullptr ) {
		getSpecularMap()->registerInStream( s );
	}

	if ( getEmissiveMap() != nullptr ) {
		getEmissiveMap()->registerInStream( s );
	}

	return true;
}

void Material::save( Stream &s )
{
	StreamObject::save( s );

	s.write( _ambient );
	s.write( _diffuse );
	s.write( _specular );
	s.write( _emissive );
	s.write( _shininess );

	s.write( _colorMap );
	s.write( _normalMap );
	s.write( _specularMap );
	s.write( _emissiveMap );
}

void Material::load( Stream &s )
{
	StreamObject::load( s );

	s.read( _ambient );
	s.read( _diffuse );
	s.read( _specular );
	s.read( _emissive );
	s.read( _shininess );

	s.read( _colorMap );
	s.read( _normalMap );
	s.read( _specularMap );
	s.read( _emissiveMap );

}

