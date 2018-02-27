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

#include "Primitive.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::Primitive )

using namespace crimild;

Primitive::Primitive( Primitive::Type type )
{
	_type = type;
}

Primitive::~Primitive( void )
{
    _indexBuffer = nullptr;
    _vertexBuffer = nullptr;
}

void Primitive::encode( coding::Encoder &encoder )
{
    Codable::encode( encoder );
    
    int type;
    switch ( _type ) {
        case Primitive::Type::POINTS:
            type = 0;
            break;
            
        case Primitive::Type::LINES:
            type = 1;
            break;
            
        case Primitive::Type::LINE_LOOP:
            type = 2;
            break;
            
        case Primitive::Type::LINE_STRIP:
            type = 3;
            break;
            
        case Primitive::Type::TRIANGLES:
            type = 4;
            break;
            
        case Primitive::Type::TRIANGLE_STRIP:
            type = 5;
            break;
            
        case Primitive::Type::TRIANGLE_FAN:
            type = 6;
            break;
    }
    
    encoder.encode( "primitiveType", type );
    
    encoder.encode( "vertexBuffer", _vertexBuffer );
    encoder.encode( "indexBuffer", _indexBuffer );
}

void Primitive::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );
    
    int type;
    decoder.decode( "primitiveType", type );
    switch ( type ) {
        case 0:
            _type = Primitive::Type::POINTS;
            break;
            
        case 1:
            _type = Primitive::Type::LINES;
            break;
            
        case 2:
            _type = Primitive::Type::LINE_LOOP;
            break;
            
        case 3:
            _type = Primitive::Type::LINE_STRIP;
            break;
            
        case 4:
            _type = Primitive::Type::TRIANGLES;
            break;
            
        case 5:
            _type = Primitive::Type::TRIANGLE_STRIP;
            break;
            
        case 6:
            _type = Primitive::Type::TRIANGLE_FAN;
            break;
    }

    decoder.decode( "vertexBuffer", _vertexBuffer );
    decoder.decode( "indexBuffer", _indexBuffer );
}

bool Primitive::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

	if ( getVertexBuffer() != nullptr ) {
		getVertexBuffer()->registerInStream( s );
	}

	if ( getIndexBuffer() != nullptr ) {
		getIndexBuffer()->registerInStream( s );
	}

	return true;
}

void Primitive::save( Stream &s )
{
	StreamObject::save( s );

	int type;
	switch ( _type ) {
		case Primitive::Type::POINTS:
			type = 0;
			break;

		case Primitive::Type::LINES:
			type = 1;
			break;

		case Primitive::Type::LINE_LOOP:
			type = 2;
			break;

		case Primitive::Type::LINE_STRIP:
			type = 3;
			break;

		case Primitive::Type::TRIANGLES:
			type = 4;
			break;

		case Primitive::Type::TRIANGLE_STRIP:
			type = 5;
			break;

		case Primitive::Type::TRIANGLE_FAN:
			type = 6;
			break;
	}

	s.write( type );

	s.write( _vertexBuffer );
	s.write( _indexBuffer );
}

void Primitive::load( Stream &s )
{
	StreamObject::load( s );

	int type;
	s.read( type );

	switch ( type ) {
		case 0:
			_type = Primitive::Type::POINTS;
			break;

		case 1:
			_type = Primitive::Type::LINES;
			break;

		case 2:
			_type = Primitive::Type::LINE_LOOP;
			break;

		case 3:
			_type = Primitive::Type::LINE_STRIP;
			break;

		case 4: 
			_type = Primitive::Type::TRIANGLES;
			break;

		case 5: 
			_type = Primitive::Type::TRIANGLE_STRIP;
			break;

		case 6:
			_type = Primitive::Type::TRIANGLE_FAN;
			break;
	}

	s.read( _vertexBuffer );
	s.read( _indexBuffer );
}

