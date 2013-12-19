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
#include "Simulation/FileSystem.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/ImageTGA.hpp"
#include "Components/MaterialComponent.hpp"

using namespace crimild;

OBJLoader::OBJLoader( std::string filePath )
	: _filePath( filePath ),
	  _currentGroup( nullptr )
{
}

OBJLoader::~OBJLoader( void )
{

}

void OBJLoader::reset( void )
{
	_positionCount = 0;
	_normalCount = 0;
	_textureCoordCount = 0;

	std::vector< std::shared_ptr< GroupDef >> empty;
	_groups.swap( empty );
	pushGroup( _filePath, _filePath );

	_materials.clear();
}

NodePtr OBJLoader::load( void )
{
	reset();

	std::ifstream input;
	input.open( _filePath.c_str() );
	if ( !input.is_open() ) {
		Log::Error << "Cannot find file " << _filePath << Log::End;
		return nullptr;
	}
	while ( !input.eof() ) {
		processLine( input );
	}

	return generateScene();
}

void OBJLoader::processLine( std::ifstream &input )
{
	std::string what;
	input >> what;

	if ( what == "v" ) {
		loadData( input, _positions, _positionCount, 3 );
	}
	else if ( what == "vn" ) {
		loadData( input, _normals, _normalCount, 3 );
	}
	else if ( what == "vt" ) {
		loadData( input, _textureCoords, _textureCoordCount, 2 );
	}
	else if ( what == "f" ) {
		// assumes a triangulated object
		std::string f0, f1, f2;
		input >> f0 >> f1 >> f2;
		_currentGroup->faces.push_back( GroupDef::Face( f0, f1, f2 ) );
		// _currentGroup->faces.push_back( f0 );
		// _currentGroup->faces.push_back( f1 );
		// _currentGroup->faces.push_back( f2 );
	}
	else if ( what == "mtllib" ) {
		std::string materialFileName;
		input >> materialFileName;
		processMaterialFile( FileSystem::getInstance().extractDirectory( _filePath ) + "/" + materialFileName );
	}
	else if ( what == "usemtl" ) {
		std::string name;
		input >> name;
		pushGroup( name, name );
	}
	else if ( what.length() > 0 ) {
		// unknown object. discard the line
		char buffer[ 1024 ];
		input.getline( buffer, 1024 );
	}
}

void OBJLoader::pushGroup( std::string name, std::string materialName )
{
	std::shared_ptr< GroupDef > group( new GroupDef( name, materialName ) );
	_groups.push_back( group );
	_currentGroup = group.get();
}

void OBJLoader::processMaterialFile( std::string materialFileName )
{
	std::ifstream materialFile;
	materialFile.open( materialFileName );
	if ( !materialFile.is_open() ) {
		Log::Error << "Cannot find material file with path " << materialFileName << Log::End;
		return;
	}

	MaterialDef *currentMaterial = nullptr;

	while ( !materialFile.eof() ) {
		char buffer[ 1024 ];
		materialFile.getline( buffer, 1024 );
		std::stringstream line;
		line << buffer;
		std::string what;
		line >> what;

		if ( what == "newmtl" ) {
			std::string materialName;
			line >> materialName;
			std::shared_ptr< MaterialDef > material( new MaterialDef( materialName ) );
			_materials[ material->name ] = material;
			currentMaterial = material.get();
		}
		else if ( what == "Ka" ) {
			float r, g, b;
			line >> r >> g >> b;
			currentMaterial->ambientColor = RGBAColorf( r, g, b, 1.0 );
		}
		else if ( what == "Kd" ) {
			float r, g, b;
			line >> r >> g >> b;
			currentMaterial->diffuseColor = RGBAColorf( r, g, b, 1.0 );
		}
		else if ( what == "Ks" ) {
			float r, g, b;
			line >> r >> g >> b;
			currentMaterial->specularColor = RGBAColorf( r, g, b, 1.0 );
		}
		else if ( what == "map_Kd" ) {
			std::string diffuseMapFileName;
			line >> diffuseMapFileName;
			if ( diffuseMapFileName.length() > 0 ) {
				Log::Debug << "Loading diffuse map " << diffuseMapFileName << Log::End;
				ImagePtr image( new ImageTGA( FileSystem::getInstance().extractDirectory( materialFileName ) + "/" + diffuseMapFileName ) );
				TexturePtr texture( new Texture( image ) );
				currentMaterial->diffuseMap = texture;
			}
		}
		else if ( what == "map_bump" ) {
			std::string normalMapFileName;
			line >> normalMapFileName;
			if ( normalMapFileName.length() > 0 ) {
				Log::Debug << "Loading normal map " << normalMapFileName << Log::End;
				ImagePtr image( new ImageTGA( FileSystem::getInstance().extractDirectory( materialFileName ) + "/" + normalMapFileName ) );
				TexturePtr texture( new Texture( image ) );
				currentMaterial->normalMap = texture;
			}
		}
		else if ( what == "map_kS" ) {
			std::string specularMapFileName;
			line >> specularMapFileName;
			if ( specularMapFileName.length() > 0 ) {
				Log::Debug << "Loading specular map " << specularMapFileName << Log::End;
				ImagePtr image( new ImageTGA( FileSystem::getInstance().extractDirectory( materialFileName ) + "/" + specularMapFileName ) );
				TexturePtr texture( new Texture( image ) );
				currentMaterial->specularMap = texture;
			}
		}
	}
}

NodePtr OBJLoader::generateScene( void )
{
	VertexFormat vf( ( _positionCount > 0 ? 3 : 0 ), 
					 0, // no color information is imported
					 ( _normalCount > 0 ? 3 : 0 ),
					 ( _normalCount > 0 && _textureCoordCount > 0 ? 3 : 0 ),
					 ( _textureCoordCount > 0 ? 2 : 0 ) );

	GroupPtr scene( new Group( _filePath ) );

	for ( auto group : _groups ) {
		if ( group->faces.size() == 0 ) {
			continue;
		}

		std::vector< float > vertices;
		std::vector< unsigned short > indices;
		unsigned short idx = 0;
		for ( auto face : group->faces ) {
			std::vector< unsigned int > faceIndices = StringUtils::split< unsigned int >( face.v0, '/' );

			std::vector< unsigned int > face0 = StringUtils::split< unsigned int >( face.v0, '/' );
			std::vector< unsigned int > face1 = StringUtils::split< unsigned int >( face.v1, '/' );
			std::vector< unsigned int > face2 = StringUtils::split< unsigned int >( face.v2, '/' );

			Vector3f p0, p1, p2;
			Vector3f n0, n1, n2;
			Vector3f tg0, tg1, tg2;
			Vector2f uv0, uv1, uv2;

			if ( vf.hasPositions() ) {
				p0[ 0 ] = _positions[ ( face0[ 0 ] - 1 ) * 3 + 0 ];
				p0[ 1 ] = _positions[ ( face0[ 0 ] - 1 ) * 3 + 1 ];
				p0[ 2 ] = _positions[ ( face0[ 0 ] - 1 ) * 3 + 2 ];

				p1[ 0 ] = _positions[ ( face1[ 0 ] - 1 ) * 3 + 0 ];
				p1[ 1 ] = _positions[ ( face1[ 0 ] - 1 ) * 3 + 1 ];
				p1[ 2 ] = _positions[ ( face1[ 0 ] - 1 ) * 3 + 2 ];

				p2[ 0 ] = _positions[ ( face2[ 0 ] - 1 ) * 3 + 0 ];
				p2[ 1 ] = _positions[ ( face2[ 0 ] - 1 ) * 3 + 1 ];
				p2[ 2 ] = _positions[ ( face2[ 0 ] - 1 ) * 3 + 2 ];
			}

			if ( vf.hasNormals() ) {
				n0[ 0 ] = _normals[ ( face0[ 2 ] - 1 ) * 3 + 0 ];
				n0[ 1 ] = _normals[ ( face0[ 2 ] - 1 ) * 3 + 1 ];
				n0[ 2 ] = _normals[ ( face0[ 2 ] - 1 ) * 3 + 2 ];

				n1[ 0 ] = _normals[ ( face1[ 2 ] - 1 ) * 3 + 0 ];
				n1[ 1 ] = _normals[ ( face1[ 2 ] - 1 ) * 3 + 1 ];
				n1[ 2 ] = _normals[ ( face1[ 2 ] - 1 ) * 3 + 2 ];

				n2[ 0 ] = _normals[ ( face2[ 2 ] - 1 ) * 3 + 0 ];
				n2[ 1 ] = _normals[ ( face2[ 2 ] - 1 ) * 3 + 1 ];
				n2[ 2 ] = _normals[ ( face2[ 2 ] - 1 ) * 3 + 2 ];
			}
	
			if ( vf.hasTextureCoords() ) {
				uv0[ 0 ] = _textureCoords[ ( face0[ 1 ] - 1 ) * 2 + 0 ];
				uv0[ 1 ] = 1.0 - _textureCoords[ ( face0[ 1 ] - 1 ) * 2 + 1 ];

				uv1[ 0 ] = _textureCoords[ ( face1[ 1 ] - 1 ) * 2 + 0 ];
				uv1[ 1 ] = 1.0 - _textureCoords[ ( face1[ 1 ] - 1 ) * 2 + 1 ];

				uv2[ 0 ] = _textureCoords[ ( face2[ 1 ] - 1 ) * 2 + 0 ];
				uv2[ 1 ] = 1.0 - _textureCoords[ ( face2[ 1 ] - 1 ) * 2 + 1 ];
			}

			if ( vf.hasTangents() ) {
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

			if ( vf.hasPositions() ) {		
				vertices.push_back( p0[ 0 ] );
				vertices.push_back( p0[ 1 ] );
				vertices.push_back( p0[ 2 ] );
			}
				
			if ( vf.hasNormals() ) {		
				vertices.push_back( n0[ 0 ] );
				vertices.push_back( n0[ 1 ] );
				vertices.push_back( n0[ 2 ] );
			}

			if ( vf.hasTangents() ) {
				vertices.push_back( tg0[ 0 ] );
				vertices.push_back( tg0[ 1 ] );
				vertices.push_back( tg0[ 2 ] );
			}
				
			if ( vf.hasTextureCoords() ) {		
				vertices.push_back( uv0[ 0 ] );
				vertices.push_back( uv0[ 1 ] );
			}
				
			if ( vf.hasPositions() ) {		
				vertices.push_back( p1[ 0 ] );
				vertices.push_back( p1[ 1 ] );
				vertices.push_back( p1[ 2 ] );
			}
				
			if ( vf.hasNormals() ) {		
				vertices.push_back( n1[ 0 ] );
				vertices.push_back( n1[ 1 ] );
				vertices.push_back( n1[ 2 ] );
			}
				
			if ( vf.hasTangents() ) {
				vertices.push_back( tg1[ 0 ] );
				vertices.push_back( tg1[ 1 ] );
				vertices.push_back( tg1[ 2 ] );
			}
				
			if ( vf.hasTextureCoords() ) {		
				vertices.push_back( uv1[ 0 ] );
				vertices.push_back( uv1[ 1 ] );
			}
				
			if ( vf.hasPositions() ) {		
				vertices.push_back( p2[ 0 ] );
				vertices.push_back( p2[ 1 ] );
				vertices.push_back( p2[ 2 ] );
			}

			if ( vf.hasNormals() ) {		
				vertices.push_back( n2[ 0 ] );
				vertices.push_back( n2[ 1 ] );
				vertices.push_back( n2[ 2 ] );
			}
			
			if ( vf.hasTangents() ) {
				vertices.push_back( tg2[ 0 ] );
				vertices.push_back( tg2[ 1 ] );
				vertices.push_back( tg2[ 2 ] );
			}
				
			if ( vf.hasTextureCoords() ) {		
				vertices.push_back( uv2[ 0 ] );
				vertices.push_back( uv2[ 1 ] );
			}

			indices.push_back( idx++ );
			indices.push_back( idx++ );
			indices.push_back( idx++ );
		}

		unsigned int vertexCount = vertices.size() / vf.getVertexSize();

		VertexBufferObjectPtr vbo( new VertexBufferObject( vf, vertexCount, &vertices[ 0 ] ) );
		IndexBufferObjectPtr ibo( new IndexBufferObject( indices.size(), &indices[ 0 ] ) );
	
		PrimitivePtr primitive( new Primitive( Primitive::Type::TRIANGLES ) );
		primitive->setVertexBuffer( vbo );
		primitive->setIndexBuffer( ibo );

		GeometryPtr geometry( new Geometry() );
		geometry->attachPrimitive( primitive );

		auto materialDef = _materials[ group->materialName ];
		if ( materialDef != nullptr ) {
			MaterialPtr material( new Material() );
			material->setAmbient( materialDef->ambientColor );
			material->setDiffuse( materialDef->diffuseColor );
			material->setSpecular( materialDef->specularColor );
			material->setColorMap( materialDef->diffuseMap );
			material->setNormalMap( materialDef->normalMap );
			material->setSpecularMap( materialDef->specularMap );
			geometry->getComponent< MaterialComponent >()->attachMaterial( material );
		}

		scene->attachNode( geometry );
	}

	return scene;
}

