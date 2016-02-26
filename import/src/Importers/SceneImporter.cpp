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

namespace crimild {

	class SkinnedMeshBone : public SharedObject {
	public:
		unsigned int id;
		Transformation offset;
	};

	class SkinnedMeshBoneCollection : public SharedObject {
	public:
		std::map< std::string, unsigned int > bones;
		std::vector< Transformation > offsets;
	};

	class SkinnedMeshAnimationChannel : public SharedObject {
	public:

	};

	class SkinnedMeshAnimation : public SharedObject {
	public:

	};

	using SkinnedMeshAnimationArray = std::vector< SharedPointer< SkinnedMeshAnimation >>;

	class SkinnedMesh : public SharedObject {
		SkinnedMeshAnimationArray animations;
	};

	class SkinnedMeshComponent : public NodeComponent {
	public:

	};

}

void computeTransform( const aiMatrix4x4 &m, Transformation &t )
{
	aiVector3D position, scaling;
 	aiQuaternion rotation;
 	m.Decompose( scaling, rotation, position );

 	t.setTranslate( position.x, position.y, position.z );
 	t.setScale( ( scaling.x + scaling.y + scaling.z ) / 3.0f );
 	t.setRotate( Quaternion4f( rotation.x, rotation.y, rotation.z, rotation.w ) );
}

class Animator : public NodeComponent {
	CRIMILD_DISALLOW_COPY_AND_ASSIGN( Animator )
	CRIMILD_NODE_COMPONENT_NAME( "Animator" )
public:

	struct AnimationBoneInfo {
		std::map< std::string, unsigned int > boneMap;
		std::vector< Transformation > boneOffsets;
	};

	template< typename T >
	struct AnimationKey {
		double time;
		T value;
	};

	using AnimationPositionKey = AnimationKey< Vector3f >;
	using AnimationRotationKey = AnimationKey< Quaternion4f >;
	using AnimationScaleKey = AnimationKey< float >;

	struct AnimationJoint {
		std::string name;
		std::vector< AnimationPositionKey > positionKeys;
		std::vector< AnimationRotationKey > rotationKeys;
		std::vector< AnimationScaleKey > scaleKeys;
	};

	using AnimationJointArray = std::vector< AnimationJoint >;
	using AnimationJointMap = std::map< std::string, AnimationJoint >;

	struct AnimationInfo {
		float duration;
		float frameRate;
		AnimationJointMap joints;
	};

	using AnimationInfoArray = std::vector< AnimationInfo >;

public:
	Animator( void ) { }

	virtual ~Animator( void ) { }

	const Transformation &getGlobalInverseTranspose( void ) const { return _globalInversePose; }
	void setGlobalInversePose( const Transformation &t ) { _globalInversePose = t; }

	const AnimationBoneInfo &getBones( void ) const { return _bones; }
	// AnimationBoneInfo &bones( void ) { return _bones; }
	void setBones( AnimationBoneInfo &bones ) { _bones = bones; }

	const AnimationInfoArray &getAnimations( void ) const { return _animations; }
	void setAnimations( const AnimationInfoArray &animations ) 
	{ 
		_animations = animations; 
		if ( _animations.size() > 0 ) {
			setCurrentAnimation( 0 );
		}
	}

	unsigned int getCurrentAnimation( void ) const { return _currentAnimation; }
	void setCurrentAnimation( unsigned int index ) 
	{ 
		_currentAnimation = index; 
		setCurrentAnimationTime( 0.0f, -1.0f );
	}

	float getCurrentAnimationStartTime( void ) const { return _currentAnimationStartTime; }
	float getCurrentAnimationEndTime( void ) const { return _currentAnimationEndTime; }
	void setCurrentAnimationTime( float start, float end ) 
	{ 
		_currentAnimationStartTime = start; 
		_currentAnimationEndTime = end; 
		if ( _currentAnimationEndTime < 0.0f ) {
			_currentAnimationEndTime = _animations[ _currentAnimation ].duration;
		}
		_currentAnimationTime = _currentAnimationStartTime;
	}

	virtual void start( void ) override
	{	
		NodeComponent::start();

		_time = 0.0f;
	}

	virtual void update( const Clock &c ) override
	{
		NodeComponent::update( c );

		/*
		_currentAnimationTime += c.getDeltaTime();
		if ( _currentAnimationTime >= _currentAnimationEndTime ) {
			_currentAnimationTime = _currentAnimationStartTime;
		}
		*/

		 _time += c.getDeltaTime();
		float timeInTicks = _time * _animations[ _currentAnimation ].frameRate;
		float animationTime = fmod( timeInTicks, _animations[ _currentAnimation ].duration );

		auto self = this;
		getNode()->perform( Apply( [self, animationTime]( Node *node ) {
			// if ( node == self->getNode() ) {
				// do nothing for the first node in the scene
				// return;
			// }

			Transformation modelTransform( node->getLocal() );

			auto &joints = self->_animations[ self->_currentAnimation ].joints;
			if ( joints.find( node->getName() ) != joints.end() ) {
				auto const &joint = joints[ node->getName() ];

				Transformation tTransform;
				Transformation rTransform;
				Transformation sTransform;

				if ( joint.positionKeys.size() > 1 ) {
					unsigned int positionIndex = 0;
					for ( int i = 0; i < joint.positionKeys.size() - 1; i++ ) {
						if ( animationTime < joint.positionKeys[ i + 1 ].time ) {
							positionIndex = i;
							break;
						}
					}
					auto const &p0 = joint.positionKeys[ positionIndex ];
					auto const &p1 = joint.positionKeys[ positionIndex + 1 ];
					float dt = p1.time - p0.time;
					float factor = ( animationTime - p0.time ) / dt;
					Interpolation::linear( p0.value, p1.value, factor, tTransform.translate() );

					// std::cout << node->getName()
					// 		  << " " << self->_time
					//  		  << " " << animationTime 
					//  		  << " " << positionIndex
					//  		  << " " << p0.time
					//  		  << " " << p1.time
					// 		  << " " << dt 
					// 		  << " " << factor
					// 		  << std::endl;
				}
				else {
					tTransform.setTranslate( joint.positionKeys[ 0 ].value );
				}

				if ( joint.rotationKeys.size() > 1 ) {
					unsigned int rotationIndex = 0;
					for ( int i = 0; i < joint.rotationKeys.size() - 1; i++ ) {
						if ( animationTime < joint.rotationKeys[ i + 1 ].time ) {
							rotationIndex = i;
							break;
						}
					}
					auto const &r0 = joint.rotationKeys[ rotationIndex ];
					auto const &r1 = joint.rotationKeys[ rotationIndex + 1 ];
					float dt = r1.time - r0.time;
					float factor = ( animationTime - r0.time ) / dt;
					rTransform.setRotate( Interpolation::slerp( r0.value, r1.value, factor ) );
				}
				else {
					rTransform.setRotate( joint.rotationKeys[ 0 ].value );
				}

				if ( joint.scaleKeys.size() > 1 ) {
					unsigned int scaleIndex = 0;
					for ( int i = 0; i < joint.scaleKeys.size() - 1; i++ ) {
						if ( animationTime < joint.scaleKeys[ i + 1 ].time ) {
							scaleIndex = i;
							break;
						}
					}
					auto const &s0 = joint.scaleKeys[ scaleIndex ];
					auto const &s1 = joint.scaleKeys[ scaleIndex + 1 ];
					float dt = s1.time - s0.time;
					float factor = ( animationTime - s0.time ) / dt;
					Interpolation::linear( s0.value, s1.value, factor, sTransform.scale() );
				}
				else {
					sTransform.setScale( joint.scaleKeys[ 0 ].value );
				}

				modelTransform.computeFrom( rTransform, sTransform );
				modelTransform.computeFrom( tTransform, modelTransform );
			}
			
			Transformation worldTransform = modelTransform;
			// worldTransform.computeFrom( node->getParent()->getWorld(), modelTransform );

			if ( false && self->_bones.boneMap.find( node->getName() ) != self->_bones.boneMap.end() ) {
				unsigned int boneIndex = self->_bones.boneMap[ node->getName() ];
				worldTransform.computeFrom( worldTransform, self->_bones.boneOffsets[ boneIndex ] );
				worldTransform.computeFrom( self->getGlobalInverseTranspose(), worldTransform );
			}

			// node->setWorld( worldTransform );
			node->setLocal( worldTransform );
			// node->setWorldIsCurrent( true );
		}));
	}

	virtual void renderDebugInfo( Renderer *renderer, Camera *camera ) override
	{
		std::vector< Vector3f > lines;
		auto self = this;
		getNode()->perform( Apply( [&lines, self]( Node *node ) {
			if ( node->hasParent() ) {
				if ( self->getBones().boneMap.find( node->getName() ) != self->getBones().boneMap.end() ) {
					lines.push_back( node->getParent()->getWorld().getTranslate() );
					lines.push_back( node->getWorld().getTranslate() );
				}
			}
		}));

		DebugRenderHelper::renderLines( renderer, camera, &lines[ 0 ], lines.size(), RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) );
	}

private:
	AnimationBoneInfo _bones;

	std::vector< AnimationInfo > _animations;
	unsigned int _currentAnimation = 0;
	float _currentAnimationTime = 0.0f;
	float _currentAnimationStartTime = 0.0f;
	float _currentAnimationEndTime = 0.0f;

	Transformation _globalInversePose;

	float _time = 0.0f;
};

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

void recursiveSceneBuilder( SharedPointer< Group > parent, const struct aiScene *s, const struct aiNode *n, std::string basePath, Animator::AnimationBoneInfo &bones ) 
{
	auto group = crimild::alloc< Group >( std::string( n->mName.data ) );
	computeTransform( n->mTransformation, group->local() );

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
			mesh->HasTextureCoords( 0 ) ? 2 : 0,
			mesh->mNumBones > 0 ? 4 : 0,
			mesh->mNumBones > 0 ? 4 : 0 
		);

		// assume all faces have the same topology
		auto primitiveType = Primitive::Type::TRIANGLES;
		const struct aiFace* face = &mesh->mFaces[ 0 ];
		switch ( face->mNumIndices ) {
			case 1: primitiveType = Primitive::Type::POINTS; break;
			case 2: primitiveType = Primitive::Type::LINES; break;
			case 3: primitiveType = Primitive::Type::TRIANGLES; break;
			// default: primitiveType = GL_POLYGON; break;
		}

		// load vertices
		const unsigned int VERTEX_COUNT = mesh->mNumVertices;
		auto vbo = crimild::alloc< VertexBufferObject >( vertexFormat, VERTEX_COUNT );

		for ( int v = 0; v < VERTEX_COUNT; v++ ) {
			vbo->setPositionAt( v, Vector3f( &mesh->mVertices[ v ].x ) );

			if ( vertexFormat.hasNormals() ) {
				vbo->setNormalAt( v, Vector3f( &mesh->mNormals[ v ].x ) );
			}

			if ( vertexFormat.hasTangents() ) {
				vbo->setTangentAt( v, Vector3f( &mesh->mTangents[ v ].x ) );
			}

			if ( vertexFormat.hasTextureCoords() ) {
				vbo->setTextureCoordAt( v, Vector2f( &mesh->mTextureCoords[ 0 ][ v ].x ) );
			}

			if ( vertexFormat.hasBoneWeights() ) {
				for ( int bw = 0; bw < vertexFormat.getBoneWeightComponents(); bw++ ) {
					vbo->setBoneIdAt( v, bw, 0.0f );
					vbo->setBoneWeightAt( v, bw, 0.0f );
				}
			}
		}

		// load indices
		const unsigned int INDEX_COUNT = mesh->mNumFaces * face->mNumIndices;
		auto ibo = crimild::alloc< IndexBufferObject >( INDEX_COUNT );

		for ( int f = 0; f < mesh->mNumFaces; f++ ) {
			const struct aiFace *face = &mesh->mFaces[ f ];

			for ( int idx = 0; idx < face->mNumIndices; idx++ ) {
				int vIdx = face->mIndices[ idx ];
				ibo->setIndexAt( f * face->mNumIndices + idx, vIdx );
			}
		}

		for ( unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++ ) {
			const aiBone *bone = mesh->mBones[ boneIdx ];
			// std::cout << "Bone: " << bone->mName.data << " W=" << bone->mNumWeights << std::endl;

			unsigned int boneIndex = 0;
			std::string boneName( bone->mName.data );

			if ( bones.boneMap.find( boneName ) == bones.boneMap.end() ) {
				boneIndex = bones.boneOffsets.size();
				bones.boneOffsets.push_back( Transformation() );
				bones.boneMap[ boneName ] = boneIndex;
			}
			else {
				boneIndex = bones.boneMap[ boneName ];
			}

			computeTransform( mesh->mBones[ boneIdx ]->mOffsetMatrix, bones.boneOffsets[ boneIndex ] );

			// bones.boneOffsets[ boneIndex ].fromMatrix( Matrix4f( mesh->mBones[ boneIndex ]->mOffsetMatrix[ 0 ] ) );
		}

		/*
		// load bones
		for ( int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++ ) {
			unsigned int boneIndex = 0;
			std::string boneName( mesh->mBones[ boneIdx ]->mName.data );

			if ( _boneMap.find( boneName ) == _boneMap.end() ) {
				boneIndex = _boneCount;
				_boneCount++;
				BoneInfo bi;
				_bones.push_back( bi );
				_boneMap[ boneName ] = boneIndex;
			}
			else {
				boneIndex = _boneMap[ boneName ];
			}

			// TODO: Transform to actual transform
			_bones[ boneIndex ].boneOffset = mesh->mBones[ boneIdx ]->mOffsetMatrix;

			for ( int weightIdx = 0; weightIdx < mesh->mBones[ boneIdx ]->mNumWeights; weightIdx++ ) {
				unsigned int vertexIdx = mesh->mBones[ boneIndex ]->mWeights[ weightIdx ].mVertexId;
				float weightValue = mesh->mBones[ boneIndex ]->mWeights[ weightIdx ].mWeight;
				for ( int vbw = 0.0f; vbw < vertexFormat.getBoneWeightComponents(); vbw++ ) {
					if ( vbo->getBoneWeightAt( vbw ) == 0.0f ) {
						vbo->setBoneIdAt( vbw, boneIndex );
						vbo->setBoneWeightAt( vbw, weightValue );
						break;
					}
				}
			}
		}
		*/

		auto primitive = crimild::alloc< Primitive >( primitiveType );
		primitive->setVertexBuffer( vbo );
		primitive->setIndexBuffer( ibo );

		auto geometry = crimild::alloc< Geometry >();
		geometry->attachPrimitive( primitive );
		group->attachNode( geometry );

		auto material = buildMaterial( s->mMaterials[ mesh->mMaterialIndex ], basePath );
		if ( material != nullptr ) {
			geometry->getComponent< MaterialComponent >()->attachMaterial( material );
		}
	}

	for ( int i = 0; i < n->mNumChildren; i++ ) {
		recursiveSceneBuilder( group, s, n->mChildren[ i ], basePath, bones );
	}

	parent->attachNode( group );
}

void loadAnimations( const aiScene *scene, Animator::AnimationInfoArray &animations )
{
	if ( scene->mNumAnimations == 0 ) {
		// nothing to load
		std::cout << "No animations" << std::endl;
		return;
	}

	for ( unsigned int aIdx = 0; aIdx < scene->mNumAnimations; aIdx++ ) {
		const aiAnimation *animation = scene->mAnimations[ aIdx ];

		Animator::AnimationInfo animationInfo;
		animationInfo.duration = animation->mDuration;
		animationInfo.frameRate = animation->mTicksPerSecond;

		std::cout << "Animation: #" << aIdx
				  << "\n\tDuration: " << animation->mDuration
				  << "\n\tFrame Rate: " << animation->mTicksPerSecond
				  << "\n\tChannels: " << animation->mNumChannels
				  << std::endl;

		for ( unsigned int cIdx = 0; cIdx < animation->mNumChannels; cIdx++ ) {
			const aiNodeAnim *channel = animation->mChannels[ cIdx ];

			std::cout << "\t *" 
					  << " Channel: " << std::string( channel->mNodeName.data )
					  << " P=" << channel->mNumPositionKeys << "(" << channel->mPositionKeys[0].mTime << " - " << channel->mPositionKeys[channel->mNumPositionKeys - 1].mTime << ")"
					  << " R=" << channel->mNumRotationKeys
					  << " S=" << channel->mNumScalingKeys
					  << std::endl;

			Animator::AnimationJoint joint;
			joint.name = std::string( channel->mNodeName.data );

			for ( int pIndex = 0; pIndex < channel->mNumPositionKeys; pIndex++ ) {
				auto pKey = channel->mPositionKeys[ pIndex ];
				joint.positionKeys.push_back( Animator::AnimationPositionKey {
					pKey.mTime,
					Vector3f( pKey.mValue.x, pKey.mValue.y, pKey.mValue.z )
				});
			}

			for ( int rIndex = 0; rIndex < channel->mNumRotationKeys; rIndex++ ) {
				auto rKey = channel->mRotationKeys[ rIndex ];
				joint.rotationKeys.push_back( Animator::AnimationRotationKey {
					rKey.mTime,
					Quaternion4f( rKey.mValue.x, rKey.mValue.y, rKey.mValue.z, rKey.mValue.w )
				});
			}

			for ( int sIndex = 0; sIndex < channel->mNumScalingKeys; sIndex++ ) {
				auto sKey = channel->mScalingKeys[ sIndex ];
				joint.scaleKeys.push_back( Animator::AnimationScaleKey {
					sKey.mTime,
					( ( sKey.mValue.x * sKey.mValue.y * sKey.mValue.z ) / 3.0f )
				});
			}

			animationInfo.joints[ joint.name ] = joint;
		}

		animations.push_back( animationInfo );
	}
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

	Animator::AnimationInfoArray animations;
	Animator::AnimationBoneInfo bones;
	loadAnimations( importedScene, animations );

	auto root = crimild::alloc< Group >( filename );
	auto basePath = FileSystem::getInstance().extractDirectory( filename ) + "/";
	recursiveSceneBuilder( root, importedScene, importedScene->mRootNode, basePath, bones );

	if ( animations.size() > 0 ) {
		auto animator = crimild::alloc< Animator >();
		animator->setAnimations( animations );
		animator->setCurrentAnimation( 0 );
		animator->setBones( bones );
		Transformation globalInversePose;
		computeTransform( importedScene->mRootNode->mTransformation.Inverse(), globalInversePose );
		animator->setGlobalInversePose( globalInversePose );
		root->attachComponent( animator );
	}

	return root;
}

