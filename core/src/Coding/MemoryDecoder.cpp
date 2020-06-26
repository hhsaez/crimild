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

#include "MemoryDecoder.hpp"
#include "EncodedData.hpp"
#include "Tags.hpp"

#include "Foundation/Log.hpp"
#include "Foundation/ObjectFactory.hpp"

#include <sstream>

using namespace crimild;
using namespace crimild::coding;

MemoryDecoder::MemoryDecoder( void )
{

}

MemoryDecoder::~MemoryDecoder( void )
{

}

crimild::Bool MemoryDecoder::decode( std::string key, SharedPointer< coding::Codable > &codable )
{
	codable = _links[ _currentObj->getUniqueID() ][ key ];
    if ( codable == nullptr ) {
        return false;
    }
	
	auto temp = _currentObj;
	_currentObj = codable;

    auto &self = *this;
	codable->decode( self );

	_currentObj = temp;

	return true;
}

crimild::Bool MemoryDecoder::decode( std::string key, std::string &value )
{
    crimild::Size l = 0;
    if ( !decode( key + "_length", l ) ) {
		return false;
	}
    
    if ( l > 0 ) {
        auto obj = crimild::cast_ptr< EncodedData >( _links[ _currentObj->getUniqueID() ][ key ] );

        ByteArray data( l + 1 );
        memcpy( &data[ 0 ], obj->getBytes().getData(), l );
        data[ l ] = '\0';
        value = std::string( ( char * ) data.getData() );
    }

	return true;
}

crimild::Size MemoryDecoder::beginDecodingArray( std::string key )
{
	crimild::Size count;
	decode( key + "_size", count );
	return count;
}

std::string MemoryDecoder::beginDecodingArrayElement( std::string key, crimild::Size index )
{
	std::stringstream ss;
	ss << key << "_" << index;
	return ss.str();
}

void MemoryDecoder::endDecodingArrayElement( std::string key, crimild::Size index )
{
	// no-op
}

void MemoryDecoder::endDecodingArray( std::string key )
{
	// no-op
}

crimild::Bool MemoryDecoder::fromBytes( const ByteArray &bytes )
{
    crimild::Size offset = 0;

	crimild::Int8 flag;
	offset += read( bytes, flag, offset );

	if ( flag != Tags::TAG_DATA_START ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format" );
		return false;
	}

    offset += read( bytes, flag, offset );
    if ( flag != Tags::TAG_DATA_VERSION ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. FLAG_VERSION expected" );
        return false;
    }
    
	std::string versionStr;
	offset += read( bytes, versionStr, offset );
	setVersion( Version( versionStr ) );

	while ( true ) {
		offset += read( bytes, flag, offset );
        if ( flag == Tags::TAG_DATA_END ) {
			break;
		}

		if ( flag == Tags::TAG_OBJECT_BEGIN ) {
			Codable::UniqueID objID;
			offset += read( bytes, objID, offset );

			std::string className;
			offset += read( bytes, className, offset );

            auto obj = crimild::dynamic_cast_ptr< Codable >( ObjectFactory::getInstance()->build( className ) );
			if ( obj == nullptr ) {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot build object of type ", className );
				return false;
			}

			_objects[ objID ] = obj;

			if ( auto encoded = crimild::dynamic_cast_ptr< EncodedData >( obj ) ) {
				ByteArray data;
				offset += read( bytes, data, offset );
				encoded->setBytes( data );
			}

			offset += read( bytes, flag, offset );
			if ( flag != Tags::TAG_OBJECT_END ) {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Expected ", Tags::TAG_OBJECT_END );
				return false;
			}
		}
		else if ( flag == Tags::TAG_LINK_BEGIN ) {
			Codable::UniqueID parentObjID;
			offset += read( bytes, parentObjID, offset );

			std::string linkName;
			offset += read( bytes, linkName, offset );

			Codable::UniqueID objID;
			offset += read( bytes, objID, offset );
			auto obj = _objects[ objID ];
			if ( obj == nullptr ) {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Cannot find object with id ", objID );
				return false;
			}

            auto parent = _objects[ parentObjID ];
            if ( parent == nullptr ) {
                Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Cannot find object with id ", objID );
                return false;
            }
			_links[ parent->getUniqueID() ][ linkName ] = obj;

			offset += read( bytes, flag, offset );
			if ( flag != Tags::TAG_LINK_END ) {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Expected ", Tags::TAG_LINK_END );
				return false;
			}
		}
		else if ( flag == Tags::TAG_ROOT_OBJECT_BEGIN ) {
			Codable::UniqueID objID;
			offset += read( bytes, objID, offset );

			auto obj = _objects[ objID ];
			if ( obj == nullptr ) {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Cannot find object with id ", objID );
				return false;
			}
            
            addRootObject( crimild::dynamic_cast_ptr< SharedObject >( obj ) );

			offset += read( bytes, flag, offset );
			if ( flag != Tags::TAG_ROOT_OBJECT_END ) {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid data format. Expected ", Tags::TAG_ROOT_OBJECT_END );
				return false;
			}
		}
		else {
			Log::error( CRIMILD_CURRENT_CLASS_NAME, "Unknown flag ", flag );
			return false;
		}
	}
    
	auto rootCount = getObjectCount();
    for ( crimild::Size i = 0; i < rootCount; i++ ) {
        auto obj = crimild::dynamic_cast_ptr< Codable >( getObjectAt< SharedObject >( i ) );
        _currentObj = obj;
        obj->decode( *this );
        _currentObj = nullptr;
    }

	return true;
}

crimild::Size MemoryDecoder::read( const ByteArray &bytes, Codable::UniqueID &value, crimild::Size offset )
{
    return readRawBytes( bytes, &value, sizeof( Codable::UniqueID ), offset );
}

crimild::Size MemoryDecoder::read( const ByteArray &bytes, crimild::Int8 &value, crimild::Size offset )
{
    return readRawBytes( bytes, &value, sizeof( crimild::Int8 ), offset );
}

crimild::Size MemoryDecoder::read( const ByteArray &bytes, std::string &value, crimild::Size offset )
{
    ByteArray buffer;
    auto readBytes = read( bytes, buffer, offset );
    value = std::string( ( const char * ) buffer.getData() );
    return readBytes;
}

crimild::Size MemoryDecoder::read( const ByteArray &bytes, ByteArray &value, crimild::Size offset )
{
    crimild::Size count;
    auto readBytes = readRawBytes( bytes, &count, sizeof( crimild::Size ), offset );
    value.resize( count );
    readBytes += readRawBytes( bytes, value.getData(), count, offset + readBytes );
    return readBytes;
}

crimild::Size MemoryDecoder::readRawBytes( const ByteArray &bytes, void *data, crimild::Size count, crimild::Size offset )
{
    memcpy( data, bytes.getData() + offset, sizeof( crimild::Byte ) * count );
    return count;
}

