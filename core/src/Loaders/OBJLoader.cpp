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

#include "OBJLoader.hpp"

#include "SceneGraph/Group.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Foundation/StringUtils.hpp"
#include "Foundation/Log.hpp"
#include "Simulation/AssetManager.hpp"
#include "Simulation/FileSystem.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/ImageTGA.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Components/MaterialComponent.hpp"

using namespace crimild;

OBJLoader::FileProcessor::FileProcessor( void )
{

}

OBJLoader::FileProcessor::~FileProcessor( void )
{

}

void OBJLoader::FileProcessor::readFile( std::string fileName )
{
	std::ifstream input;
	input.open( fileName.c_str() );
	if ( !input.is_open() ) {
		Log::Error << "Cannot load OBJ file " << fileName << Log::End;
		return;
	}

	while ( !input.eof() ) {
		processLine( input );
	}
}

void OBJLoader::FileProcessor::registerLineProcessor( std::string type, OBJLoader::FileProcessor::LineProcessor lineProcessor )
{
	_lineProcessors[ type ] = lineProcessor;
}

std::string OBJLoader::FileProcessor::getLine( std::ifstream &input )
{
	char buffer[ 1024 ];
	input.getline( buffer, 1024 );
	return std::string( buffer );
}

void OBJLoader::FileProcessor::processLine( std::ifstream &input )
{
	std::stringstream line( getLine( input ) );
	
	std::string what;
	line >> what;

	auto processor = _lineProcessors[ what ];
	if ( processor != nullptr ) {
		processor( line );
	}
}

OBJLoader::OBJLoader( std::string fileName )
	: _fileName( fileName )
{
	getOBJProcessor().registerLineProcessor( "o", std::bind( &OBJLoader::readObject, this, std::placeholders::_1 ) );
	getOBJProcessor().registerLineProcessor( "g", std::bind( &OBJLoader::readObject, this, std::placeholders::_1 ) );
	getOBJProcessor().registerLineProcessor( "v", std::bind( &OBJLoader::readObjectPositions, this, std::placeholders::_1 ) );
	getOBJProcessor().registerLineProcessor( "vn", std::bind( &OBJLoader::readObjectNormals, this, std::placeholders::_1 ) );
	getOBJProcessor().registerLineProcessor( "vt", std::bind( &OBJLoader::readObjectTextureCoords, this, std::placeholders::_1 ) );
	getOBJProcessor().registerLineProcessor( "f", std::bind( &OBJLoader::readObjectFaces, this, std::placeholders::_1 ) );
 	getOBJProcessor().registerLineProcessor( "usemtl", std::bind( &OBJLoader::readObjectMaterial, this, std::placeholders::_1 ) );
	getOBJProcessor().registerLineProcessor( "mtllib", std::bind( &OBJLoader::readMaterialFile, this, std::placeholders::_1 ) );

	getMTLProcessor().registerLineProcessor( "newmtl", std::bind( &OBJLoader::readMaterialName, this, std::placeholders::_1 ) );
	getMTLProcessor().registerLineProcessor( "Ka", std::bind( &OBJLoader::readMaterialAmbient, this, std::placeholders::_1 ) );
	getMTLProcessor().registerLineProcessor( "Kd", std::bind( &OBJLoader::readMaterialDiffuse, this, std::placeholders::_1 ) );
	getMTLProcessor().registerLineProcessor( "Ks", std::bind( &OBJLoader::readMaterialSpecular, this, std::placeholders::_1 ) );
	getMTLProcessor().registerLineProcessor( "map_Kd", std::bind( &OBJLoader::readMaterialColorMap, this, std::placeholders::_1 ) );
	getMTLProcessor().registerLineProcessor( "map_bump", std::bind( &OBJLoader::readMaterialNormalMap, this, std::placeholders::_1 ) );
	getMTLProcessor().registerLineProcessor( "map_kS", std::bind( &OBJLoader::readMaterialSpecularMap, this, std::placeholders::_1 ) );
	getMTLProcessor().registerLineProcessor( "map_Ke", std::bind( &OBJLoader::readMaterialEmissiveMap, this, std::placeholders::_1 ) );
	getMTLProcessor().registerLineProcessor( "illum", std::bind( &OBJLoader::readMaterialShaderProgram, this, std::placeholders::_1 ) );
}

OBJLoader::~OBJLoader( void )
{

}

void OBJLoader::reset( void )
{
	_currentObject = nullptr;
	_objects.clear();

	_currentMaterial = nullptr;
	_materials.clear();

	_positions.clear();
	_normals.clear();
	_textureCoords.clear();
}

SharedPointer< Group > OBJLoader::load( void )
{
	reset();

	getOBJProcessor().readFile( getFileName() );

	return generateScene();
}

void OBJLoader::generateGeometry( void )
{
	if ( _faces.size() == 0 || _positions.size() == 0 ) {
		// no data. skip
		return;
	}

	if ( _currentObject == nullptr ) {
		// anonymous object
        _objects.push_back( std::move( crimild::alloc< Group >() ) );
        _currentObject = crimild::get_ptr( _objects.back() );
                           
	}

	VertexFormat format( 3,
						 0,
						 ( _normals.size() > 0 ? 3 : 0 ),
	                     0, //( _normalCount > 0 && _textureCoordCount > 0 ? 3 : 0 ),
						 ( _textureCoords.size() > 0 ? 2 : 0 ) );

	std::vector< float > vertexData;
	std::vector< unsigned short > indexData;

	for ( auto face : _faces ) {
		std::vector< int > f = StringUtils::split< int >( face, '/' );

		if ( format.hasPositions() ) {
            const Vector3f &position = _positions[ f[ 0 ] - 1 ];
			vertexData.push_back( position[ 0 ] );
			vertexData.push_back( position[ 1 ] );
			vertexData.push_back( position[ 2 ] );
		}

		if ( format.hasNormals() ) {
            const Vector3f &normal = _normals[ f[ 2 ] - 1 ];
			vertexData.push_back( normal[ 0 ] );
			vertexData.push_back( normal[ 1 ] );
			vertexData.push_back( normal[ 2 ] );
		}

/*
		if ( format.hasTangents() ) {
#if 1
			Vector3f g;

			g = Vector3f( 1, 0, 0 );
			if ( ( g ^ n0 ).getSquaredMagnitude() < Numericf::ZERO_TOLERANCE ) {
				//g = Vector3f( 0.0f, 1.0f, 0.0f );
			}
			if ( ( g ^ n0 ).getSquaredMagnitude() < Numericf::ZERO_TOLERANCE ) {
				//g = Vector3f( 0.0f, 0.0f, 1.0f );
			}
			tg0 = ( n0 ^ g );

			g = Vector3f( 1, 0, 0 );
			if ( ( g ^ n1 ).getSquaredMagnitude() < Numericf::ZERO_TOLERANCE ) {
				//g = Vector3f( 0.0f, 1.0f, 0.0f );
			}
			if ( ( g ^ n1 ).getSquaredMagnitude() < Numericf::ZERO_TOLERANCE ) {
				//g = Vector3f( 0.0f, 0.0f, 1.0f );
			}
			tg1 = ( n1 ^ g );

			g = Vector3f( 1, 0, 0 );
			if ( ( g ^ n2 ).getSquaredMagnitude() < Numericf::ZERO_TOLERANCE ) {
				//g = Vector3f( 0.0f, 1.0f, 0.0f );
			}
			if ( ( g ^ n2 ).getSquaredMagnitude() < Numericf::ZERO_TOLERANCE ) {
				//g = Vector3f( 0.0f, 0.0f, 1.0f );
			}
			tg2 = ( n2 ^ g );
#else
				float coef = 1.0 / ( uv0[ 0 ] * uv1[ 1 ] - uv1[ 0 ] * uv0[ 1 ] );
				Vector3f tangent;
				tangent[ 0 ] = coef * ( ( p0[ 0 ] * uv1[ 1 ] ) + ( p1[ 0 ] * -uv0[ 1 ] ) );
				tangent[ 1 ] = coef * ( ( p0[ 1 ] * uv1[ 1 ] ) + ( p1[ 1 ] * -uv0[ 1 ] ) );
				tangent[ 2 ] = coef * ( ( p0[ 2 ] * uv1[ 1 ] ) + ( p1[ 2 ] * -uv0[ 1 ] ) );				
				tangent.normalize();
				tg0 = tg1 = tg2 = tangent;
#endif
				//tg0 = tg1 = tg2 = ( p1 - p0 );
				//tg1 = ( p2 - p1 );
				//tg2 = ( p0 - p2 );
			}
*/

		if ( format.hasTextureCoords() ) {
            const Vector2f &uv = _textureCoords[ f[ 1 ] - 1 ];
			vertexData.push_back( uv[ 0 ] );
			vertexData.push_back( uv[ 1 ] );
		}

		indexData.push_back( indexData.size() );
	}

	int vertexCount = indexData.size();

	auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
	primitive->setVertexBuffer( crimild::alloc< VertexBufferObject >( format, vertexCount, &vertexData[ 0 ] ) );
	primitive->setIndexBuffer( crimild::alloc< IndexBufferObject >( indexData.size(), &indexData[ 0 ] ) );

	auto geometry = crimild::alloc< Geometry >( "geometry" );
	geometry->attachPrimitive( primitive );

	if ( _currentMaterial ) {
		geometry->getComponent< MaterialComponent >()->attachMaterial( _currentMaterial );	
	}

	_currentObject->attachNode( geometry );

	_faces.clear();
}

SharedPointer< Group > OBJLoader::generateScene( void )
{
	// DON'T FORGET THE LAST OBJECT!!
	generateGeometry();

	auto scene = crimild::alloc< Group >( getFileName() );
	for ( auto obj : _objects ) {
		scene->attachNode( obj );
	}

	return scene;
}

void OBJLoader::readObject( std::stringstream &line )
{
	generateGeometry();

	std::string name;
	line >> name;

    _objects.push_back( std::move( crimild::alloc< Group >( name ) ) );
    _currentObject = crimild::get_ptr( _objects.back() );
}

void OBJLoader::readObjectPositions( std::stringstream &line )
{
	float x, y, z;
	line >> x >> y >> z;
	_positions.push_back( Vector3f( x, y, z ) );
}

void OBJLoader::readObjectTextureCoords( std::stringstream &line )
{
	float s, t;
	line >> s >> t;
	_textureCoords.push_back( Vector2f( s, t ) );
}

void OBJLoader::readObjectNormals( std::stringstream &line ) 
{
	float x, y, z;
	line >> x >> y >> z;
	_normals.push_back( Vector3f( x, y, z ) );
}

void OBJLoader::readObjectFaces( std::stringstream &line )
{
	std::string f0, f1, f2;
	line >> f0 >> f1 >> f2;
	_faces.push_back( f0 );
	_faces.push_back( f1 );
	_faces.push_back( f2 );
}

void OBJLoader::readObjectMaterial( std::stringstream &line )
{
	std::string name;
	line >> name;
    _currentMaterial = crimild::get_ptr( _materials[ name ] );
}

void OBJLoader::readMaterialFile( std::stringstream &line )
{
	std::string mtlFileName;
	line >> mtlFileName;
	while ( !line.eof() ) {
		// handle spaces in mtl file name
		std::string temp;
		line >> temp;
		mtlFileName += " ";
		mtlFileName += temp;
	}

	std::string mtlFilePath = FileSystem::getInstance().extractDirectory( _fileName ) + "/" + mtlFileName;
	getMTLProcessor().readFile( mtlFilePath );
	_currentMaterial = nullptr;
}

void OBJLoader::readMaterialName( std::stringstream &line )
{
	std::string name;
	line >> name;

    auto tmp = std::move( crimild::alloc< Material >() );
    _materials[ name ] = tmp;
    _currentMaterial = crimild::get_ptr( tmp );
}

void OBJLoader::readMaterialAmbient( std::stringstream &line )
{
	float r, g, b;
	line >> r >> g >> b;
	_currentMaterial->setAmbient( RGBAColorf( r, g, b, 1.0f ) );
}

void OBJLoader::readMaterialDiffuse( std::stringstream &line )
{
	float r, g, b;
	line >> r >> g >> b;
	_currentMaterial->setDiffuse( RGBAColorf( r, g, b, 1.0f ) );
}

void OBJLoader::readMaterialSpecular( std::stringstream &line )
{
	float r, g, b;
	line >> r >> g >> b;
	_currentMaterial->setSpecular( RGBAColorf( r, g, b, 1.0f ) );
}

void OBJLoader::readMaterialColorMap( std::stringstream &line )
{
	std::string fileName;
	line >> fileName;
    _currentMaterial->setColorMap( loadTexture( fileName ) );
}

void OBJLoader::readMaterialNormalMap( std::stringstream &line )
{
	std::string fileName;
	line >> fileName;
	_currentMaterial->setNormalMap( loadTexture( fileName ) );
}

void OBJLoader::readMaterialSpecularMap( std::stringstream &line )
{
	std::string fileName;
	line >> fileName;
	_currentMaterial->setSpecularMap( loadTexture( fileName ) );
}

void OBJLoader::readMaterialEmissiveMap( std::stringstream &line )
{
	std::string fileName;
	line >> fileName;
	_currentMaterial->setEmissiveMap( loadTexture( fileName ) );
}

void OBJLoader::readMaterialShaderProgram( std::stringstream &line )
{
	int illumLevel;
	line >> illumLevel;

	switch ( illumLevel ) {
	    case 0:
            _currentMaterial->setProgram( AssetManager::getInstance()->get< ShaderProgram >( Renderer::SHADER_PROGRAM_UNLIT_TEXTURE ) );
			break;

        case 1:
            _currentMaterial->setProgram( AssetManager::getInstance()->get< ShaderProgram >( Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE ) );
            break;

        case 3:
            // receive shadows, but no casting
            _currentMaterial->setCastShadows( false );
            _currentMaterial->setReceiveShadows( true );
            break;
            
        case 4:
            // neither receive nor cast shadows
            _currentMaterial->setCastShadows( false );
            _currentMaterial->setReceiveShadows( false );
            break;
    };
}

SharedPointer< Texture > OBJLoader::loadTexture( std::string textureFileName )
{
	if ( textureFileName == "" ) {
		return nullptr;
	}
    auto image = crimild::alloc< ImageTGA >( FileSystem::getInstance().extractDirectory( _fileName ) + "/" + textureFileName );
    auto texture = crimild::alloc< Texture >( image );
    return texture;
}

