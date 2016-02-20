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

#include "SceneImporter.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

using namespace crimild;
using namespace crimild::import;

SharedPointer< Material > buildMaterial( const aiMaterial *mtl, std::string basePath )
{
	auto material = crimild::alloc< Material >();

	int texIndex = 0;
	unsigned int max = 1;

	aiString texPath;
	if ( AI_SUCCESS == mtl->GetTexture( aiTextureType_DIFFUSE, texIndex, &texPath ) ) {
		auto texturePath = basePath + texPath.data;
		auto texture = AssetManager::getInstance()->get< Texture >( texturePath );
		if ( texture != nullptr ) {
			material->setColorMap( texture );
		}
	}

	if ( AI_SUCCESS == mtl->GetTexture( aiTextureType_SPECULAR, texIndex, &texPath ) ) {
		auto texturePath = basePath + texPath.data;
		auto texture = AssetManager::getInstance()->get< Texture >( texturePath );
		if ( texture != nullptr ) {
			material->setSpecularMap( texture );
		}
	}

	if ( AI_SUCCESS == mtl->GetTexture( aiTextureType_HEIGHT, texIndex, &texPath ) ) {
		auto texturePath = basePath + texPath.data;
		auto texture = AssetManager::getInstance()->get< Texture >( texturePath );
		if ( texture != nullptr ) {
			material->setNormalMap( texture );
		}
	}

	if ( AI_SUCCESS == mtl->GetTexture( aiTextureType_NORMALS, texIndex, &texPath ) ) {
		auto texturePath = basePath + texPath.data;
		auto texture = AssetManager::getInstance()->get< Texture >( texturePath );
		if ( texture != nullptr ) {
			material->setNormalMap( texture );
		}
	}

	aiColor4D color;

	if ( AI_SUCCESS == aiGetMaterialColor( mtl, AI_MATKEY_COLOR_AMBIENT, &color ) ) {
		material->setAmbient( RGBAColorf( color.r, color.g, color.b, color.a ) );
	}

	if ( AI_SUCCESS == aiGetMaterialColor( mtl, AI_MATKEY_COLOR_DIFFUSE, &color ) ) {
		material->setDiffuse( RGBAColorf( color.r, color.g, color.b, color.a ) );
	}

	if ( AI_SUCCESS == aiGetMaterialColor( mtl, AI_MATKEY_COLOR_SPECULAR, &color ) ) {
		material->setSpecular( RGBAColorf( color.r, color.g, color.b, color.a ) );
	}

	if ( AI_SUCCESS == aiGetMaterialColor( mtl, AI_MATKEY_COLOR_EMISSIVE, &color ) ) {
		// average
		material->setEmissive( ( color.r + color.g + color.b ) / 3.0f );
	}

	float shininess, strength;
	max = 1;
	if ( AI_SUCCESS == aiGetMaterialFloatArray( mtl, AI_MATKEY_SHININESS, &shininess, &max ) &&
		 AI_SUCCESS == aiGetMaterialFloatArray( mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max ) ) {
		material->setShininess( shininess * strength );
	}

	int two_sided;
	max = 1;
	if ( AI_SUCCESS == aiGetMaterialIntegerArray( mtl, AI_MATKEY_TWOSIDED, &two_sided, &max ) ) {
		material->getCullFaceState()->setEnabled( two_sided );
	}

	return material;
}

void recursiveSceneBuilder( SharedPointer< Group > parent, const struct aiScene *s, const struct aiNode *n, std::string basePath ) 
{
	auto group = crimild::alloc< Group >();

	aiVector3D position, scaling;
 	aiQuaternion rotation;
 	n->mTransformation.Decompose( scaling, rotation, position );

 	group->local().setTranslate( position.x, position.y, position.z );
 	group->local().setScale( scaling.x );
 	group->local().setRotate( Quaternion4f( rotation.x, rotation.y, rotation.z, rotation.w ) );

	for ( int i = 0; i < n->mNumMeshes; i++ ) {
		const struct aiMesh *mesh = s->mMeshes[ n->mMeshes[ i ] ];

		if ( mesh->mNumFaces == 0 ) {
			continue;
		}

		VertexFormat vertexFormat( 
			3,
			0,//mesh->mColors[0] != nullptr ? 4 : 0,
			mesh->mNormals != nullptr ? 3 : 0,
			mesh->mTangents != nullptr ? 3 : 0,
			mesh->HasTextureCoords( 0 ) ? 2 : 0 );

		auto primitiveType = Primitive::Type::TRIANGLES;
		const struct aiFace* face = &mesh->mFaces[ 0 ];
		switch ( face->mNumIndices ) {
			case 1: primitiveType = Primitive::Type::POINTS; break;
			case 2: primitiveType = Primitive::Type::LINES; break;
			case 3: primitiveType = Primitive::Type::TRIANGLES; break;
			// default: primitiveType = GL_POLYGON; break;
		}

		std::vector< float > vertices;
		std::vector< unsigned short > indices;

		for ( int f = 0; f < mesh->mNumFaces; f++ ) {
			const struct aiFace *face = &mesh->mFaces[ f ];

			for ( int idx = 0; idx < face->mNumIndices; idx++ ) {
				int vIdx = face->mIndices[ idx ];

				indices.push_back( indices.size() );

				vertices.push_back( mesh->mVertices[ vIdx ].x );
				vertices.push_back( mesh->mVertices[ vIdx ].y );
				vertices.push_back( mesh->mVertices[ vIdx ].z );

				if ( vertexFormat.hasNormals() ) {
					vertices.push_back( mesh->mNormals[ vIdx ].x );
					vertices.push_back( mesh->mNormals[ vIdx ].y );
					vertices.push_back( mesh->mNormals[ vIdx ].z );
				}

				if ( vertexFormat.hasTangents() ) {
					vertices.push_back( mesh->mTangents[ vIdx].x );
					vertices.push_back( mesh->mTangents[ vIdx].y );
					vertices.push_back( mesh->mTangents[ vIdx].z );
				}

				if ( vertexFormat.hasTextureCoords() ) {
					vertices.push_back( mesh->mTextureCoords[ 0 ][ vIdx ].x );
					vertices.push_back( mesh->mTextureCoords[ 0 ][ vIdx ].y );
				}
			}

		}

		if ( indices.size() >= std::numeric_limits<unsigned short>::max() ) {
			std::cout << "Vertices: " << indices.size() << "(max: " << std::numeric_limits<unsigned short>::max() << ")" << std::endl;
		}

		auto primitive = crimild::alloc< Primitive >( primitiveType );
		primitive->setVertexBuffer( crimild::alloc< VertexBufferObject >( vertexFormat, indices.size(), &vertices[ 0 ] ) );
		primitive->setIndexBuffer( crimild::alloc< IndexBufferObject >( indices.size(), &indices[ 0 ] ) );

		auto geometry = crimild::alloc< Geometry >();
		geometry->attachPrimitive( primitive );
		group->attachNode( geometry );

		auto material = buildMaterial( s->mMaterials[ mesh->mMaterialIndex ], basePath );
		if ( material != nullptr ) {
			geometry->getComponent< MaterialComponent >()->attachMaterial( material );
		}
	}

	for ( int i = 0; i < n->mNumChildren; i++ ) {
		recursiveSceneBuilder( group, s, n->mChildren[ i ], basePath );
	}

	parent->attachNode( group );
}

SceneImporter::SceneImporter( void )
{

}

SceneImporter::~SceneImporter( void )
{

}

SharedPointer< Group > SceneImporter::import( std::string filename )
{
	// check if file exists
	std::ifstream fin( filename );
	bool exists = !fin.fail();
	fin.close();
	if ( !exists ) {
		// Is it ok to throw exceptions?
		throw FileNotFoundException( filename );
	}

	Assimp::Importer importer;
	const aiScene* importedScene = importer.ReadFile( filename, 
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate );
	if ( importedScene == nullptr ) {
		Log::Error << "Error importing file " << filename
				   << "\n" << importer.GetErrorString()
				   << Log::End;
	 	return nullptr;
	}

	auto root = crimild::alloc< Group >( filename );
	auto basePath = FileSystem::getInstance().extractDirectory( filename ) + "/";
	recursiveSceneBuilder( root, importedScene, importedScene->mRootNode, basePath );

	return root;
}

