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
#include "Version.hpp"
#include "Log.hpp"

#include "Rendering/VertexFormat.hpp"

using namespace crimild;

bool StreamObject::registerInStream( Stream &s )
{
	return s.registerObject( this );
}

void StreamObject::save( Stream &s )
{
	s.write( getClassName() );
	s.write( getUniqueIdentifier() );
}

void StreamObject::load( Stream &s )
{

}

SharedPointer< StreamObject > StreamObjectFactory::buildObject( std::string className )
{
    if ( _builders[ className ] == nullptr ) {
        Log::Error << "No builder registred for StreamObject of class " << className << Log::End;
        return nullptr;
    }

    return _builders[ className ]();
}

Stream::Stream( void )
{

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
	return registerObject( obj->getUniqueIdentifier(), crimild::retain( obj ) );
}

bool Stream::registerObject( StreamObject::StreamObjectId objId, SharedPointer< StreamObject > const &obj )
{
	if ( _objects[ objId ] != nullptr ) {
		// object already register
		return false;
	}

	_objects[ objId ] = obj;

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
	write( Version::getDescription() );

	for ( auto &obj : _topLevelObjects ) {
		if ( obj != nullptr ) {
			obj->registerInStream( *this );
		}
	}

	for ( auto &it : _objects ) {
		auto &obj = it.second;
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
		Log::Error << "Invalid file format";
		return false;
	}

	std::string version;
	read( version );

	while ( true ) {
		read( flag );
		if ( flag == Stream::FLAG_STREAM_END ) {
			break;
		}

		bool topLevel = ( flag == Stream::FLAG_TOP_LEVEL_OBJECT );

		read( flag );
		if ( flag != Stream::FLAG_OBJECT_START ) {
			Log::Error << "Invalid file format. Expected " << Stream::FLAG_OBJECT_START << Log::End;
			return false;
		}

		std::string className;
		read( className );

		StreamObject::StreamObjectId objId;
		read( objId );

		auto obj = StreamObjectFactory::getInstance()->buildObject( className );
		if ( obj == nullptr ) {
			return false;
		}

		obj->load( *this );

		if ( topLevel ) {
			addObject( obj );
		}

		registerObject( objId, obj );

		read( flag );
		if ( flag != Stream::FLAG_OBJECT_END ) {
			Log::Error << "Invalid file format. Expected " << Stream::FLAG_OBJECT_END << Log::End;
			return false;
		}
	}

	for ( auto &action : _deferredActions ) {
		action( *this );
	}

	return true;
}

void Stream::write( const std::string &str )
{
	write( str.c_str() );
}

void Stream::write( const char *str )
{
	write( strlen( str ) );
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

void Stream::writeChildObject( StreamObject *o )
{
	if ( o != nullptr ) {
		write( o->getUniqueIdentifier() );
	}
	else {
		StreamObject::StreamObjectId objId = 0;
		write( objId );
	}
}

void Stream::writeChildObjects( std::vector< StreamObject * > &os )
{
	size_t count = os.size();
	write( count );
	for ( auto &o : os ) {
		write( o->getUniqueIdentifier() );
	}
}

void Stream::read( std::string &str )
{
	size_t count = 0;
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

FileStream::FileStream( std::string path, FileStream::OpenMode openMode )
	: _path( path ),
	  _openMode( openMode ),
	  _file( nullptr )
{

}

FileStream::~FileStream( void )
{
	close();
}

bool FileStream::open( void )
{
	close();

	_file = fopen( _path.c_str(), _openMode == FileStream::OpenMode::WRITE ? "w" : "r" );
	if ( _file == nullptr ) {
		Log::Error << "Invalid file path " << _path;
		return false;
	}

	return true;
}

bool FileStream::close( void )
{
	if ( _file != nullptr ) {
		fclose( _file );
		_file = nullptr;
	}	

	return true;
}

bool FileStream::flush( void )
{
	open();
	auto result = Stream::flush();
	close();

	return result;
}

bool FileStream::load( void )
{
	open();
	auto result = Stream::load();
	close();

	return result;
}

void FileStream::writeRawBytes( const void *bytes, size_t size )
{
	fwrite( bytes, 1, size, _file );
}

void FileStream::readRawBytes( void *bytes, size_t size )
{
	fread( bytes, 1, size, _file );
}

