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

#include "Stream.hpp"

#include "Foundation/Version.hpp"
#include "Foundation/Log.hpp"
#include "Rendering/VertexFormat.hpp"
#include "Rendering/SkinnedMesh.hpp"
#include "Components/SkinnedMeshComponent.hpp"

#include <algorithm>

using namespace crimild;

constexpr const char *Stream::FLAG_STREAM_START;
constexpr const char *Stream::FLAG_STREAM_END;
constexpr const char *Stream::FLAG_TOP_LEVEL_OBJECT;
constexpr const char *Stream::FLAG_INNER_OBJECT;
constexpr const char *Stream::FLAG_OBJECT_START;
constexpr const char *Stream::FLAG_OBJECT_END;

bool StreamObject::registerInStream( Stream &s )
{
	return s.registerObject( this );
}

void StreamObject::save( Stream &s )
{
	s.write( dynamic_cast< RTTI * >( this )->getClassName() );
	s.write( getUniqueIdentifier() );
}

void StreamObject::load( Stream &s )
{

}

SharedPointer< StreamObject > StreamObjectFactory::buildObject( std::string className )
{
    if ( _builders[ className ] == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No builder registred for StreamObject of class ", className );
        return nullptr;
    }

    return _builders[ className ]();
}

Stream::Stream( void )
{
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMeshJoint );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMeshJointCatalog );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMeshAnimationChannel );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMeshAnimationClip );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMeshSkeleton );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMeshAnimationState );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMesh );
CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMeshComponent )



}

Stream::~Stream( void )
{

}

bool Stream::isTopLevel( SharedPointer< StreamObject > const &obj ) const 
{
	for ( const auto &other : _topLevelObjects ) {
		// double check
		if ( other == obj && other->getUniqueIdentifier() == obj->getUniqueIdentifier() ) {
			return true;
		}
	}

	return false;
}

bool Stream::registerObject( StreamObject *obj )
{
    return registerObject( obj->getUniqueIdentifier(), crimild::dynamic_cast_ptr< StreamObject >( crimild::retain( dynamic_cast< SharedObject * >( obj ) ) ) );
}

bool Stream::registerObject( StreamObject::StreamObjectId objId, SharedPointer< StreamObject > const &obj )
{
	if ( _objects[ objId ] != nullptr ) {
		// object already register, remove it so it will be reinserted
		// again with a higher priority
		_orderedObjects.remove( obj );
	}

	_objects[ objId ] = obj;
	_orderedObjects.push_back( obj );

	return true;
}

void Stream::addObject( SharedPointer< StreamObject > const &obj )
{
	if ( isTopLevel( obj ) ) {
		return;
	}

	_topLevelObjects.push_back( obj );
}

bool Stream::flush( void )
{
	write( Stream::FLAG_STREAM_START );

	write( _version.getDescription() );

	for ( auto &obj : _topLevelObjects ) {
		if ( obj != nullptr ) {
			obj->registerInStream( *this );
		}
	}

	for ( auto it = _orderedObjects.rbegin(); it != _orderedObjects.rend(); it++ ) {
		auto &obj = *it;
		if ( obj != nullptr ) {
			if ( isTopLevel( obj ) ) {
				write( Stream::FLAG_TOP_LEVEL_OBJECT );
			}
			else {
				write( Stream::FLAG_INNER_OBJECT );
			}

			write( Stream::FLAG_OBJECT_START );
			obj->save( *this );
			write( Stream::FLAG_OBJECT_END );
		}
	}

	write( Stream::FLAG_STREAM_END );

	return true;
}

bool Stream::load( void )
{
	std::string flag;
	read( flag );
	if ( flag != Stream::FLAG_STREAM_START ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid file format" );
		return false;
	}

	std::string versionStr;
	read( versionStr );
	_version.fromString( versionStr );

	while ( true ) {
		read( flag );
		if ( flag == Stream::FLAG_STREAM_END ) {
			break;
		}

		bool topLevel = ( flag == Stream::FLAG_TOP_LEVEL_OBJECT );

		read( flag );
		if ( flag != Stream::FLAG_OBJECT_START ) {
            Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid file format. Expected ", Stream::FLAG_OBJECT_START );
			return false;
		}

		std::string className;
		read( className );

		StreamObject::StreamObjectId objId;
		read( objId );

		auto obj = StreamObjectFactory::getInstance()->buildObject( className );
		if ( obj == nullptr ) {
            Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Cannot build object of type ", className, " with id ", objId );
			return false;
		}

		obj->load( *this );

		if ( topLevel ) {
			addObject( obj );
		}

		registerObject( objId, obj );

		read( flag );
		if ( flag != Stream::FLAG_OBJECT_END ) {
            Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid file format. Expected ", Stream::FLAG_OBJECT_END );
			return false;
		}
	}

	return true;
}

void Stream::write( const std::string &str )
{
	write( str.c_str() );
}

void Stream::write( const char *str )
{
	write( ( unsigned int ) strlen( str ) );
	writeRawBytes( str, strlen( str ) );
}

void Stream::write( const VertexFormat &vf )
{
	write( vf.getPositionComponents() );
	write( vf.getColorComponents() );
	write( vf.getNormalComponents() );
	write( vf.getTangentComponents() );
	write( vf.getTextureCoordComponents() );
	write( vf.getBoneIdComponents() );
	write( vf.getBoneWeightComponents() );
}

void Stream::write( const Quaternion4f &q )
{
	write( q.getRawData() );
}

void Stream::write( const Transformation &t )
{
	write( t.getTranslate() );
	write( t.getRotate() );
	write( t.getScale() );
}

void Stream::write( char c )
{
	writeRawBytes( &c, sizeof( char ) );
}

void Stream::write( unsigned char c )
{
	writeRawBytes( &c, sizeof( unsigned char ) );
}

void Stream::write( short s )
{
	writeRawBytes( &s, sizeof( short ) );
}

void Stream::write( unsigned short s )
{
	writeRawBytes( &s, sizeof( unsigned short ) );
}

void Stream::write( int i )
{
	writeRawBytes( &i, sizeof( int ) );
}

void Stream::write( unsigned int i )
{
	writeRawBytes( &i, sizeof( unsigned int ) );
}

void Stream::write( long long ll )
{
	writeRawBytes( &ll, sizeof( long long ) );
}

void Stream::write( unsigned long long ll )
{
	writeRawBytes( &ll, sizeof( unsigned long long ) );
}

void Stream::write( float f )
{
	writeRawBytes( &f, sizeof( float ) );
}

void Stream::read( std::string &str )
{
	unsigned int count = 0;
	read( count );
	if ( count == 0 ) {
		str = "";
		return;
	}

	std::vector< char > buffer( count + 1 );
	readRawBytes( &buffer[ 0 ], count );
	buffer[ count ] = '\0';

	str = std::string( ( const char * ) &buffer[ 0 ] );
}

void Stream::read( VertexFormat &vf )
{
	unsigned char positions;
	read( positions );

	unsigned char colors;
	read( colors );

	unsigned char normals;
	read( normals );

	unsigned char tangents;
	read( tangents );

	unsigned char textureCoords;
	read( textureCoords );

	unsigned char boneIds;
	read( boneIds );

	unsigned char boneWeights;
	read( boneWeights );

	vf = VertexFormat( positions, colors, normals, tangents, textureCoords, boneIds, boneWeights );
}

void Stream::read( Quaternion4f &q )
{
	Vector4f v;
	read( v );
	q = Quaternion4f( v );
}

void Stream::read( Transformation &t )
{
	read( t.translate() );
	read( t.rotate() );
	read( t.scale() );
}

void Stream::read( char &c )
{
	readRawBytes( &c, sizeof( char ) );
}

void Stream::read( unsigned char &c )
{
	readRawBytes( &c, sizeof( unsigned char ) );
}

void Stream::read( short &s )
{
	readRawBytes( &s, sizeof( short ) );
}

void Stream::read( unsigned short &s )
{
	readRawBytes( &s, sizeof( unsigned short ) );
}

void Stream::read( int &i )
{
	readRawBytes( &i, sizeof( int ) );
}

void Stream::read( unsigned int &i )
{
	readRawBytes( &i, sizeof( unsigned int ) );
}

void Stream::read( long long &i )
{
	readRawBytes( &i, sizeof( long long ) );
}

void Stream::read( unsigned long long &i )
{
	readRawBytes( &i, sizeof( unsigned long long ) );
}

void Stream::read( float &f )
{
	readRawBytes( &f, sizeof( float ) );
}

