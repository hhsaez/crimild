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

#include "Components/RenderStateComponent.hpp"
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
#include "Rendering/Programs/UnlitShaderProgram.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/ShaderLibrary.hpp"
#include "Rendering/ShaderProgramLibrary.hpp"
#include "Rendering/Uniforms/ModelViewProjectionUniformBuffer.hpp"
#include "Components/MaterialComponent.hpp"

namespace crimild {

    namespace utils {

        /**
           \see https://stackoverflow.com/a/35991300
        */
        template< typename T >
        inline void hash_combine( std::size_t &seed, const T &v )
        {
            std::hash< T > hasher;
            seed ^= hasher( v ) + 0x934779b9 + ( seed << 6 ) + ( seed >> 2 );
        }

    }

}

namespace std {

    template<> struct hash< crimild::Vector3f > {
        size_t operator()( crimild::Vector3f const &v ) const {
            size_t seed = 0;
            std::hash< float > hasher;
            crimild::utils::hash_combine( seed, hasher( v.x() ) );
            crimild::utils::hash_combine( seed, hasher( v.y() ) );
            crimild::utils::hash_combine( seed, hasher( v.z() ) );
            return seed;
        }
    };

    template<> struct hash< crimild::Vector2f > {
        size_t operator()( crimild::Vector2f const &v ) const {
            size_t seed = 0;
            std::hash< float > hasher;
            crimild::utils::hash_combine( seed, hasher( v.x() ) );
            crimild::utils::hash_combine( seed, hasher( v.y() ) );
            return seed;
        }
    };

    template<> struct hash< crimild::VertexP3N3TC2 > {
        size_t operator()( crimild::VertexP3N3TC2 const &vertex ) const {
            size_t seed = 0;
            crimild::utils::hash_combine( seed, std::hash< crimild::Vector3f >()( vertex.position ) );
            crimild::utils::hash_combine( seed, std::hash< crimild::Vector3f >()( vertex.normal ) );
            crimild::utils::hash_combine( seed, std::hash< crimild::Vector2f >()( vertex.texCoord ) );
            return seed;
        }
    };

}

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
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot load file ", fileName );
		return;
	}

	while ( !input.eof() ) {
		processLine( input );
	}
}

void OBJLoader::FileProcessor::registerLineProcessor( std::string type, OBJLoader::FileProcessor::LineProcessor lineProcessor )
{
    _lineProcessors[ StringUtils::toLower( type ) ] = lineProcessor;
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

    auto processor = _lineProcessors[ StringUtils::toLower( what ) ];
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
    getMTLProcessor().registerLineProcessor( "d", std::bind( &OBJLoader::readMaterialTranslucency, this, std::placeholders::_1 ) );
    getMTLProcessor().registerLineProcessor( "Tr", std::bind( &OBJLoader::readMaterialTranslucency, this, std::placeholders::_1 ) );
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
    if ( pipeline == nullptr ) {
        pipeline = crimild::alloc< Pipeline >();
        pipeline->program = crimild::retain( ShaderProgramLibrary::getInstance()->get( constants::SHADER_PROGRAM_UNLIT_TEXTURE_P3N3TC2 ) );
    }

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
        _objects.push_back( crimild::alloc< Group >() );
        _currentObject = crimild::get_ptr( _objects.back() );
                           
	}
    
    std::vector< VertexP3N3TC2 > vertices;
    std::vector< crimild::UInt32 > indices;
    std::unordered_map< VertexP3N3TC2, crimild::UInt32 > uniqueVertices;

    for ( auto f = 0l; f < _faces.size(); f += 3 ) {
        for ( int i = 0; i < 3; i++ ) {
            auto faceIndices = StringUtils::split< crimild::Int32 >( _faces[ f + i ], '/' );

            auto v = VertexP3N3TC2 {
                .position = _positions[ faceIndices[ 0 ] - 1 ],
                .normal = !_normals.empty() ? _normals[ faceIndices[ 2 ] - 1 ] : Vector3f::ZERO,
                .texCoord = !_textureCoords.empty() ? _textureCoords[ faceIndices[ 1 ] - 1 ] : Vector2f::ZERO,
            };

            if ( uniqueVertices.count( v ) == 0 ) {
                uniqueVertices[ v ] = vertices.size();
                vertices.push_back( v );
            }

            indices.push_back( uniqueVertices[ v ] );
        }
    }

    auto vbo = crimild::alloc< VertexP3N3TC2Buffer >( containers::Array< VertexP3N3TC2 >( vertices.size(), vertices.data() ) );
    auto ibo = crimild::alloc< IndexBuffer< crimild::UInt32 >>( containers::Array< crimild::UInt32 >( indices.size(), indices.data() ) );

    _currentObject->attachNode( [&] {
        auto geometry = crimild::alloc< Geometry >( "geometry" );
        geometry->attachComponent( [&] {
            auto renderState = crimild::alloc< RenderStateComponent >();
            renderState->pipeline = pipeline;
            renderState->vbo = vbo;
            renderState->ibo = ibo;
            renderState->uniforms = {
                [&] {
                    auto ubo = crimild::alloc< ModelViewProjectionUniformBuffer >();
                    ubo->node = crimild::get_ptr( geometry );
                    return ubo;
                }(),
            };
            renderState->textures = {
                [&] {
                    if ( _currentMaterial == nullptr || _currentMaterial->getColorMap() == nullptr ) {
                        return Texture::INVALID;
                    }
                    return crimild::retain( _currentMaterial->getColorMap() );
                }(),
            };
            return renderState;
        }());

        return geometry;
    }());

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

    _objects.push_back( crimild::alloc< Group >( name ) );
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
    std::string mtlFileName = StringUtils::readFullString( line );
	std::string mtlFilePath = FileSystem::getInstance().extractDirectory( _fileName ) + "/" + mtlFileName;

	getMTLProcessor().readFile( mtlFilePath );
	_currentMaterial = nullptr;
}

void OBJLoader::readMaterialName( std::stringstream &line )
{
	std::string name;
	line >> name;

    auto tmp =  crimild::alloc< Material >() ;
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
    _currentMaterial->setColorMap( loadTexture( StringUtils::readFullString( line ) ) );
}

void OBJLoader::readMaterialNormalMap( std::stringstream &line )
{
	_currentMaterial->setNormalMap( loadTexture( StringUtils::readFullString( line ) ) );
}

void OBJLoader::readMaterialSpecularMap( std::stringstream &line )
{
	_currentMaterial->setSpecularMap( loadTexture( StringUtils::readFullString( line ) ) );
}

void OBJLoader::readMaterialEmissiveMap( std::stringstream &line )
{
    _currentMaterial->setEmissiveMap( loadTexture( StringUtils::readFullString( line ) ) );
}

void OBJLoader::readMaterialShaderProgram( std::stringstream &line )
{
	int illumLevel;
	line >> illumLevel;

	switch ( illumLevel ) {
	    case 0:
            _currentMaterial->setProgram( crimild::alloc< UnlitShaderProgram >() );
			break;

        case 1:
            _currentMaterial->setProgram( crimild::alloc< UnlitShaderProgram >() );
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

void OBJLoader::readMaterialTranslucency( std::stringstream &line )
{
    float translucency;
    line >> translucency;
    
    if ( translucency < 1.0f ) {
        _currentMaterial->getAlphaState()->setEnabled( true );
        auto diffuse = _currentMaterial->getDiffuse();
        diffuse[ 3 ] = translucency;
        _currentMaterial->setDiffuse( diffuse );
    }
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

