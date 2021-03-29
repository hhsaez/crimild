/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holders nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "SceneImporter.hpp"

#include "Animation/ChannelImpl.hpp"
#include "Animation/Clip.hpp"
#include "Animation/Skeleton.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/SkinnedMeshComponent.hpp"
#include "Exceptions/FileNotFoundException.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/ImageManager.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Materials/LitMaterial.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/SkinnedMesh.hpp"
#include "Rendering/Texture.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Simulation/AssetManager.hpp"
#include "Simulation/FileSystem.hpp"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "assimp/postprocess.h"

using namespace crimild;
using namespace crimild::import;

SharedPointer< Group > SceneImporter::importScene( std::string filename )
{
    SceneImporter importer;
    return importer.import( filename );
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
    importer.SetPropertyInteger( AI_CONFIG_PP_SLM_VERTEX_LIMIT, 15000 );
    const aiScene *importedScene = importer.ReadFile( filename, aiProcessPreset_TargetRealtime_MaxQuality );
    if ( importedScene == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Error importing file ", filename, "\n", importer.GetErrorString() );
        return nullptr;
    }

    SharedPointer< SkinnedMesh > skinnedMesh;

    /*
    auto skinnedMesh = crimild::alloc< SkinnedMesh >();
    loadAnimations( importedScene, skinnedMesh );
    */

    auto root = crimild::alloc< Group >( filename );
    auto basePath = FileSystem::getInstance().extractDirectory( filename ) + "/";
    recursiveSceneBuilder( root, importedScene, importedScene->mRootNode, basePath, skinnedMesh );

    if ( _skeleton != nullptr ) {
        Transformation globalInverseTransform;
        computeTransform( importedScene->mRootNode->mTransformation.Inverse(), globalInverseTransform );
        _skeleton->setGlobalInverseTransform( globalInverseTransform );
        root->attachComponent( _skeleton );
    }

    return root;
}

animation::Joint *SceneImporter::getJoint( std::string name )
{
    if ( _joints.contains( name ) ) {
        return crimild::get_ptr( _joints[ name ] );
    }

    auto jointId = _joints.size();
    auto joint = crimild::alloc< animation::Joint >( name, jointId );
    _joints[ name ] = joint;

    return crimild::get_ptr( joint );
}

void SceneImporter::computeTransform( const aiMatrix4x4 &m, Transformation &t )
{
    aiVector3D position, scaling;
    aiQuaternion rotation;
    m.Decompose( scaling, rotation, position );

    t.setTranslate( position.x, position.y, position.z );
    t.setScale( ( scaling.x + scaling.y + scaling.z ) / 3.0f );
    t.setRotate( Quaternion4f( rotation.x, rotation.y, rotation.z, rotation.w ) );
}

void SceneImporter::loadMaterialTexture( SharedPointer< LitMaterial > material, const aiMaterial *input, std::string basePath, aiTextureType texType, unsigned int texIndex )
{
    if ( !input->GetTextureCount( texType ) ) {
        return;
    }

    aiString texPath;
    if ( AI_SUCCESS == input->GetTexture( texType, texIndex, &texPath ) ) {
        // assume textures are in the same directory as model.
        auto fileName = FileSystem::getInstance().getFileName( texPath.data );
        auto texturePath = basePath + fileName;

        auto image = ImageManager::getInstance()->loadImage(
            { .filePath = {
                  .path = texturePath,
                  .pathType = FilePath::PathType::ABSOLUTE,
              } } );

        if ( image == nullptr ) {
            CRIMILD_LOG_WARNING( "Failed to load image ", texturePath );
            image = Image::INVALID;
        }

        auto texture = crimild::alloc< Texture >();
        texture->imageView = crimild::alloc< ImageView >();
        texture->imageView->image = image;
        texture->sampler = [] {
            auto sampler = crimild::alloc< Sampler >();
            sampler->setMinFilter( Sampler::Filter::LINEAR );
            sampler->setMagFilter( Sampler::Filter::LINEAR );
            return sampler;
        }();

        if ( texture != nullptr ) {
            switch ( texType ) {
                case aiTextureType_DIFFUSE:
                case aiTextureType_BASE_COLOR:
                    material->setAlbedoMap( texture );
                    break;

                case aiTextureType_DIFFUSE_ROUGHNESS:
                    material->setRoughnessMap( texture );
                    break;

                case aiTextureType_METALNESS:
                    material->setMetallicMap( texture );
                    break;

                case aiTextureType_LIGHTMAP:
					material->setAmbientOcclusionMap( texture );
                    break;

                case aiTextureType_HEIGHT:
                case aiTextureType_NORMALS:
                    material->setNormalMap( texture );
                    break;

                case aiTextureType_UNKNOWN:
                	// assume Matal/Roughness
                    material->setCombinedRoughnessMetallicMap( texture );
                	break;

                default:
                    Log::warning( CRIMILD_CURRENT_CLASS_NAME, "Unsupported texture type ", texType );
                    break;
            }
        } else {
            Log::warning( CRIMILD_CURRENT_CLASS_NAME, "Cannot find texture with path ", texturePath );
        }
    }
}

SharedPointer< LitMaterial > SceneImporter::buildMaterial( const aiMaterial *mtl, std::string basePath )
{
    auto material = crimild::alloc< LitMaterial >();

    unsigned int max = 1;

    for ( auto textureTypeIdx = UInt32( aiTextureType_DIFFUSE ); textureTypeIdx <= aiTextureType_UNKNOWN; ++textureTypeIdx ) {
        const auto textureType = ( aiTextureType ) textureTypeIdx;
        const auto maxTextures = mtl->GetTextureCount( textureType );
        for ( auto textureCount = 0; textureCount < maxTextures; ++textureCount ) {
            loadMaterialTexture( material, mtl, basePath, textureType, textureCount );
        }
    }

    aiColor4D color;

    if ( AI_SUCCESS == aiGetMaterialColor( mtl, AI_MATKEY_COLOR_AMBIENT, &color ) ) {
        //material->setAmbient( RGBAColorf( color.r, color.g, color.b, color.a ) );
    }

    if ( AI_SUCCESS == aiGetMaterialColor( mtl, AI_MATKEY_COLOR_DIFFUSE, &color ) ) {
        //material->setDiffuse( RGBAColorf( color.r, color.g, color.b, color.a ) );
        material->setAlbedo( RGBColorf( color.r, color.g, color.b ) );
    }

    if ( AI_SUCCESS == aiGetMaterialColor( mtl, AI_MATKEY_COLOR_SPECULAR, &color ) ) {
        //material->setSpecular( RGBAColorf( color.r, color.g, color.b, color.a ) );
    }

    if ( AI_SUCCESS == aiGetMaterialColor( mtl, AI_MATKEY_COLOR_EMISSIVE, &color ) ) {
        // average
        auto e = ( color.r + color.g + color.b ) / 3.0f;
        if ( e > 0.0f ) {
            //material->setEmissive( e );
        }
    }

    float shininess, strength;
    max = 1;
    if ( AI_SUCCESS == aiGetMaterialFloatArray( mtl, AI_MATKEY_SHININESS, &shininess, &max ) && AI_SUCCESS == aiGetMaterialFloatArray( mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max ) ) {
        //material->setShininess( shininess * strength );
    }

    int two_sided;
    max = 1;
    if ( AI_SUCCESS == aiGetMaterialIntegerArray( mtl, AI_MATKEY_TWOSIDED, &two_sided, &max ) ) {
        //material->getCullFaceState()->setEnabled( two_sided );
    }

    crimild::Real32 opacity;
    max = 1;
    if ( AI_SUCCESS == aiGetMaterialFloatArray( mtl, AI_MATKEY_OPACITY, &opacity, &max ) ) {
        if ( opacity < 1.0f ) {
            //auto d = material->getDiffuse();
            //d[ 3 ] = opacity;
            //material->setDiffuse( d );
            //material->setAlphaState( crimild::alloc< AlphaState >( true ) );
        }
    }

    return material;
}

void SceneImporter::recursiveSceneBuilder( SharedPointer< Group > parent, const struct aiScene *s, const struct aiNode *n, std::string basePath, SharedPointer< SkinnedMesh > &skinnedMesh )
{
    auto group = crimild::alloc< Group >( std::string( n->mName.data ) );
    computeTransform( n->mTransformation, group->local() );

    /*
	auto joint = getJoint( group->getName() );
	if ( joint != nullptr ) {
		group->attachComponent( joint );
	}
    */

    for ( int i = 0; i < n->mNumMeshes; i++ ) {
        const struct aiMesh *mesh = s->mMeshes[ n->mMeshes[ i ] ];

        if ( mesh->mNumFaces == 0 ) {
            continue;
        }

        auto vertexAttributes = Array< VertexAttribute > {
            // always has positions
            { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
        };

        if ( mesh->mNormals ) {
            vertexAttributes.add( { VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() } );
        }

        if ( mesh->HasTextureCoords( 0 ) ) {
            vertexAttributes.add( { VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() } );
        }

        // TODO: add bone attributes

        // VertexFormat vertexFormat(
        //     3,
        //     0, //mesh->mColors[0] != nullptr ? 4 : 0,
        //     mesh->mNormals != nullptr ? 3 : 0,
        //     mesh->mTangents != nullptr ? 3 : 0,
        //     mesh->HasTextureCoords( 0 ) ? 2 : 0,
        //     mesh->mNumBones > 0 ? 4 : 0,
        //     mesh->mNumBones > 0 ? 4 : 0 );

        const struct aiFace *face = &mesh->mFaces[ 0 ];

        auto primitiveType = [ indicesPerFace = face->mNumIndices ] {
            switch ( indicesPerFace ) {
                case 1: {
                    return Primitive::Type::POINTS;
                }

                case 2: {
                    return Primitive::Type::LINES;
                }

                default:
                case 3: {
                    return Primitive::Type::TRIANGLES;
                }
            }
        }();

        // load vertices
        auto vertexLayout = VertexLayout( vertexAttributes );
        const UInt32 VERTEX_COUNT = mesh->mNumVertices;
        auto vbo = crimild::alloc< VertexBuffer >( vertexLayout, VERTEX_COUNT );
        auto positions = vbo->get( VertexAttribute::Name::POSITION );
        auto normals = vbo->get( VertexAttribute::Name::NORMAL );
        auto texCoords = vbo->get( VertexAttribute::Name::TEX_COORD );

        // TODO: instead of iterating over all vertices, we should be able to
        // use buffer accessors to set buffer data directly like this:
        // positions->set( Array( mesh->mVertices ) );
        for ( auto v = 0l; v < VERTEX_COUNT; v++ ) {
            positions->set( v, Vector3f( &mesh->mVertices[ v ].x ) );

            if ( normals != nullptr ) {
                normals->set( v, Vector3f( &mesh->mNormals[ v ].x ) );
            }

            if ( texCoords != nullptr ) {
                auto uv = Vector2f( &mesh->mTextureCoords[ 0 ][ v ].x );
                uv.y() = 1.0f - uv.y();
                texCoords->set( v, uv );
            }

            // vbo->setPositionAt( v, Vector3f( &mesh->mVertices[ v ].x ) );

            // if ( vertexFormat.hasNormals() ) {
            //     vbo->setNormalAt( v, Vector3f( &mesh->mNormals[ v ].x ) );
            // }

            // if ( vertexFormat.hasTangents() ) {
            //     vbo->setTangentAt( v, Vector3f( &mesh->mTangents[ v ].x ) );
            // }

            // if ( vertexFormat.hasTextureCoords() ) {
            //     vbo->setTextureCoordAt( v, Vector2f( &mesh->mTextureCoords[ 0 ][ v ].x ) );
            // }

            // if ( vertexFormat.hasBoneWeights() ) {
            //     for ( int bw = 0; bw < vertexFormat.getBoneWeightComponents(); bw++ ) {
            //         vbo->setBoneIdAt( v, bw, 0.0f );
            //         vbo->setBoneWeightAt( v, bw, 0.0f );
            //     }
            // }
        }

        // load indices
        const auto INDEX_COUNT = Size( mesh->mNumFaces * face->mNumIndices );
        auto ibo = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, INDEX_COUNT );
        for ( int f = 0; f < mesh->mNumFaces; f++ ) {
            const struct aiFace *face = &mesh->mFaces[ f ];
            for ( int idx = 0; idx < face->mNumIndices; idx++ ) {
                int vIdx = face->mIndices[ idx ];
                ibo->setIndex( f * face->mNumIndices + idx, vIdx );
                // ibo->setIndexAt( f * face->mNumIndices + idx, vIdx );
            }
        }

        /*
        for ( unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++ ) {
            const aiBone *bone = mesh->mBones[ boneIdx ];

            Transformation offset;
            computeTransform( mesh->mBones[ boneIdx ]->mOffsetMatrix, offset );
            auto jointName = std::string( bone->mName.data );
            auto joint = getJoint( jointName );
            joint->setOffset( offset );

            for ( int weightIdx = 0; weightIdx < mesh->mBones[ boneIdx ]->mNumWeights; weightIdx++ ) {
                unsigned int vertexIdx = mesh->mBones[ boneIdx ]->mWeights[ weightIdx ].mVertexId;
                float weightValue = mesh->mBones[ boneIdx ]->mWeights[ weightIdx ].mWeight;
                for ( int vbw = 0.0f; vbw < vertexFormat.getBoneWeightComponents(); vbw++ ) {
                    if ( vbo->getBoneWeightAt( vertexIdx, vbw ) == 0.0f ) {
                        vbo->setBoneIdAt( vertexIdx, vbw, joint->getId() );
                        vbo->setBoneWeightAt( vertexIdx, vbw, weightValue );
                        break;
                    }
                }
            }
        }
        */

        auto primitive = crimild::alloc< Primitive >( primitiveType );
        primitive->setVertexData( { vbo } );
        primitive->setIndices( ibo );

        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( primitive );
        group->attachNode( geometry );

        auto material = buildMaterial( s->mMaterials[ mesh->mMaterialIndex ], basePath );
        if ( material != nullptr ) {
            geometry->getComponent< MaterialComponent >()->attachMaterial( material );
        }
    }

    for ( int i = 0; i < n->mNumChildren; i++ ) {
        recursiveSceneBuilder( group, s, n->mChildren[ i ], basePath, skinnedMesh );
    }

    parent->attachNode( group );
}

void SceneImporter::loadAnimations( const aiScene *scene, SharedPointer< SkinnedMesh > &skinnedMesh )
{
    if ( scene->mNumAnimations == 0 ) {
        // nothing to load
        return;
    }

    _skeleton = crimild::alloc< animation::Skeleton >();

    for ( crimild::Size aIdx = 0; aIdx < scene->mNumAnimations; aIdx++ ) {
        const auto *inAnimation = scene->mAnimations[ aIdx ];

        auto clip = crimild::alloc< animation::Clip >( std::string( inAnimation->mName.data ) );
        clip->setDuration( inAnimation->mDuration );
        clip->setFrameRate( inAnimation->mTicksPerSecond );

        for ( unsigned int cIdx = 0; cIdx < inAnimation->mNumChannels; cIdx++ ) {
            const aiNodeAnim *inChannel = inAnimation->mChannels[ cIdx ];

            // Positions
            Array< crimild::Real32 > pChannelTimes( inChannel->mNumPositionKeys );
            Array< Vector3f > pChannelKeys( inChannel->mNumPositionKeys );
            for ( crimild::Size pIndex = 0; pIndex < inChannel->mNumPositionKeys; pIndex++ ) {
                auto pKey = inChannel->mPositionKeys[ pIndex ];
                pChannelKeys[ pIndex ] = Vector3f( pKey.mValue.x, pKey.mValue.y, pKey.mValue.z );
                pChannelTimes[ pIndex ] = pKey.mTime;
            }
            auto pChannel = crimild::alloc< animation::Vector3fChannel >(
                std::string( inChannel->mNodeName.data ) + "[p]",
                std::move( pChannelTimes ),
                std::move( pChannelKeys ) );
            clip->addChannel( pChannel );

            // Rotations
            Array< crimild::Real32 > rChannelTimes( inChannel->mNumRotationKeys );
            Array< Quaternion4f > rChannelKeys( inChannel->mNumRotationKeys );
            for ( crimild::Size rIndex = 0; rIndex < inChannel->mNumRotationKeys; rIndex++ ) {
                auto rKey = inChannel->mRotationKeys[ rIndex ];
                Quaternion4f q( rKey.mValue.x, rKey.mValue.y, rKey.mValue.z, rKey.mValue.w );
                if ( false && rIndex > 0 ) {
                    // fix orientation
                    auto &q0 = rChannelKeys[ rIndex - 1 ];
                    if ( q.getRawData() * q0.getRawData() < 0 ) {
                        q = -q;
                    }
                }
                rChannelKeys[ rIndex ] = q;
                rChannelTimes[ rIndex ] = rKey.mTime;
            }
            auto rChannel = crimild::alloc< animation::Quaternion4fChannel >(
                std::string( inChannel->mNodeName.data ) + "[r]",
                std::move( rChannelTimes ),
                std::move( rChannelKeys ) );
            clip->addChannel( rChannel );

            // Scaling
            const auto INV_3 = 1.0f / 3.0f;
            Array< crimild::Real32 > sChannelTimes( inChannel->mNumScalingKeys );
            Array< crimild::Real32 > sChannelKeys( inChannel->mNumScalingKeys );
            for ( crimild::Size sIndex = 0; sIndex < inChannel->mNumScalingKeys; sIndex++ ) {
                auto sKey = inChannel->mScalingKeys[ sIndex ];
                // compute uniform scaling by averaging all values
                sChannelKeys[ sIndex ] = ( sKey.mValue.x + sKey.mValue.y + sKey.mValue.z ) * INV_3;
                sChannelTimes[ sIndex ] = sKey.mTime;
            }
            auto sChannel = crimild::alloc< animation::Real32Channel >(
                std::string( inChannel->mNodeName.data ) + "[s]",
                std::move( sChannelTimes ),
                std::move( sChannelKeys ) );
            clip->addChannel( sChannel );
        }

        _skeleton->getClips()[ clip->getName() ] = clip;
    }
}
