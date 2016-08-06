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

#ifndef CRIMILD_RENDERING_SKINNED_MESH_
#define CRIMILD_RENDERING_SKINNED_MESH_

#include "Foundation/NamedObject.hpp"
#include "Foundation/Stream.hpp"
#include "Foundation/Map.hpp"
#include "Foundation/Array.hpp"

#include "Mathematics/Transformation.hpp"
#include "Mathematics/Interpolation.hpp"

namespace crimild {
    
	class SkinnedMeshJoint : public StreamObject, public NamedObject {
		CRIMILD_IMPLEMENT_RTTI( crimild::SkinnedMeshJoint )

	public:
		SkinnedMeshJoint( void );
		SkinnedMeshJoint( unsigned int id, const Transformation &offset, std::string name );
		virtual ~SkinnedMeshJoint( void );

		unsigned int getId( void ) const { return _id; }

		void setOffset( const Transformation &t ) { _offset = t; }
		const Transformation &getOffset( void ) const { return _offset; }

	private:
		unsigned char _id;
		Transformation _offset;

		/**
			\name Streaming
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}

	};

	class SkinnedMeshJointCatalog : public StreamObject {
		CRIMILD_IMPLEMENT_RTTI( crimild::SkinnedMeshJointCatalog )

	public:
		SkinnedMeshJointCatalog( void );
		virtual ~SkinnedMeshJointCatalog( void );

		unsigned int getJointCount( void ) const { return _joints.size(); }

		SkinnedMeshJoint *find( std::string name );

		SkinnedMeshJoint *updateOrCreateJoint( std::string name, const Transformation &offset );

	private:
		Map< std::string, SharedPointer< SkinnedMeshJoint >> _joints;

		/**
			\name Streaming
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}

	};

	class SkinnedMeshAnimationChannel : public NamedObject, public StreamObject {
		CRIMILD_IMPLEMENT_RTTI( crimild::SkinnedMeshAnimationChannel )

	public:
		template< typename T >
		struct AnimationKey {
			double time;
			T value;
		};

		using PositionKey = AnimationKey< Vector3f >;
		using PositionKeyArray = Array< PositionKey >;

		using RotationKey = AnimationKey< Quaternion4f >;
		using RotationKeyArray = Array< RotationKey >;

		using ScaleKey = AnimationKey< float >;
		using ScaleKeyArray = Array< ScaleKey >;

	public:
		SkinnedMeshAnimationChannel( void );
		virtual ~SkinnedMeshAnimationChannel( void );

		void setPositionKeys( PositionKeyArray const &keys ) { _positionKeys = keys; }
		PositionKeyArray &getPositionKeys( void ) { return _positionKeys; }
		const PositionKeyArray &getPositionKeys( void ) const { return _positionKeys; }

		void setRotationKeys( RotationKeyArray const &keys ) { _rotationKeys = keys; }
		RotationKeyArray &getRotationKeys( void ) { return _rotationKeys; }
		const RotationKeyArray &getRotationKeys( void ) const { return _rotationKeys; }

		void setScaleKeys( ScaleKeyArray const &keys ) { _scaleKeys = keys; }
		ScaleKeyArray &getScaleKeys( void ) { return _scaleKeys; }
		const ScaleKeyArray &getScaleKeys( void ) const { return _scaleKeys; }

		bool computePosition( float animationTime, Vector3f &result );
		bool computeRotation( float animationTime, Quaternion4f &result );
		bool computeScale( float animationTime, float &result );

	private:
		PositionKeyArray _positionKeys;
		RotationKeyArray _rotationKeys;
		ScaleKeyArray _scaleKeys;

		/**
			\name Streaming
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}		
	};

	class SkinnedMeshAnimationClip : public StreamObject {
		CRIMILD_IMPLEMENT_RTTI( crimild::SkinnedMeshAnimationClip )

	private:
		using SkinnedMeshAnimationChannelMap = Map< std::string, SharedPointer< SkinnedMeshAnimationChannel >>;

	public:
		SkinnedMeshAnimationClip( void );
		virtual ~SkinnedMeshAnimationClip( void );

		float getDuration( void ) const { return _duration; }
		void setDuration( float duration ) { _duration = duration; }

		float getFrameRate( void ) const { return _frameRate; }
		void setFrameRate( float frameRate ) { _frameRate = frameRate; }

		SkinnedMeshAnimationChannelMap &getChannels( void ) { return _channels; }

	private:
		float _duration;
		float _frameRate;
		SkinnedMeshAnimationChannelMap _channels;

		/**
			\name Streaming
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}		
	};

	class SkinnedMeshSkeleton : public StreamObject {
		CRIMILD_IMPLEMENT_RTTI( crimild::SkinnedMeshSkeleton )

	private:
		using SkinnedMeshAnimationClipArray = Array< SharedPointer< SkinnedMeshAnimationClip >>;

	public:
		SkinnedMeshSkeleton( void );
		virtual ~SkinnedMeshSkeleton( void );

		SkinnedMeshAnimationClipArray &getClips( void ) { return _clips; }
		SkinnedMeshJointCatalog *getJoints( void ) { return crimild::get_ptr( _joints ); }

		void setGlobalInverseTransform( const Transformation &value ) { _globalInverseTransform = value; }
		const Transformation &getGlobalInverseTransform( void ) const { return _globalInverseTransform; }

	private:
		SkinnedMeshAnimationClipArray _clips;
		SharedPointer< SkinnedMeshJointCatalog > _joints;

		Transformation _globalInverseTransform;

		/**
			\name Streaming
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}		
	};

	class SkinnedMeshAnimationState : public StreamObject {
		CRIMILD_IMPLEMENT_RTTI( crimild::SkinnedMeshAnimationState )

	private:
		using JointPoseArray = Array< Matrix4f >;

	public:
		SkinnedMeshAnimationState( void );
		virtual ~SkinnedMeshAnimationState( void );

		JointPoseArray &getJointPoses( void ) { return _jointPoses; }
		const JointPoseArray &getJointPoses( void ) const { return _jointPoses; }

	private:
		JointPoseArray _jointPoses;

		/**
			\name Streaming
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}		
	};

	class SkinnedMesh : public StreamObject {
		CRIMILD_IMPLEMENT_RTTI( crimild::SkinnedMesh )

	public:
		SkinnedMesh( void );
		virtual ~SkinnedMesh( void );

		SharedPointer< SkinnedMesh > clone( void );

		SkinnedMeshSkeleton *getSkeleton( void ) { return crimild::get_ptr( _skeleton ); }
		void setSkeleton( SharedPointer< SkinnedMeshSkeleton > const &skeleton ) { _skeleton = skeleton; }

		SkinnedMeshAnimationState *getAnimationState( void ) { return crimild::get_ptr( _animationState ); }

	private:
		SharedPointer< SkinnedMeshSkeleton > _skeleton;
		SharedPointer< SkinnedMeshAnimationState > _animationState;

		/**
			\name Streaming
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}

	public:
		void debugDump( void );
	};

}

#endif

