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

#include "SkinnedMesh.hpp"

using namespace crimild;

SkinnedMeshJoint::SkinnedMeshJoint( void ) 
{ 

}

SkinnedMeshJoint::SkinnedMeshJoint( unsigned int id, const Transformation &offset )
	: _id( id ), 
	  _offset( offset ) 
{ 

}

SkinnedMeshJoint::~SkinnedMeshJoint( void ) 
{ 

}

SkinnedMeshJointCatalog::SkinnedMeshJointCatalog( void ) 
{ 

}

SkinnedMeshJointCatalog::~SkinnedMeshJointCatalog( void ) 
{ 

}

SkinnedMeshJoint *SkinnedMeshJointCatalog::find( std::string name )
{			
	if ( !_lookup.find( name ) ) {
		return nullptr;
	}

	return crimild::get_ptr( _joints[ _lookup[ name ] ] );
}

SkinnedMeshJoint *SkinnedMeshJointCatalog::updateOrCreateJoint( std::string name, const Transformation &offset )
{
	if ( _lookup.find( name ) ) {
		auto id = _lookup[ name ];
		auto joint = _joints[ id ];
		joint->setOffset( offset );
		return crimild::get_ptr( joint );
	}

	unsigned int jointId = _joints.size();
	auto joint = crimild::alloc< SkinnedMeshJoint >( jointId, offset );
	_joints.add( joint );
	_lookup[ name ] = jointId;

	return crimild::get_ptr( joint );
}

SkinnedMeshAnimationChannel::SkinnedMeshAnimationChannel( void )
{

}

SkinnedMeshAnimationChannel::~SkinnedMeshAnimationChannel( void )
{
	
}

bool SkinnedMeshAnimationChannel::computePosition( float animationTime, Vector3f &result )
{
	if ( getPositionKeys().size() == 1 ) {
		result = getPositionKeys()[ 0 ].value;
		return true;
	}

	unsigned int positionIndex = 0;
	for ( int i = 0; i < getPositionKeys().size() - 1; i++ ) {
		if ( animationTime < getPositionKeys()[ i + 1 ].time ) {
			positionIndex = i;
			break;
		}
	}

	auto const &p0 = getPositionKeys()[ positionIndex ];
	auto const &p1 = getPositionKeys()[ positionIndex + 1 ];
	float dt = p1.time - p0.time;
	float factor = ( animationTime - p0.time ) / dt;
	Interpolation::linear( p0.value, p1.value, factor, result );
	return true;
}

bool SkinnedMeshAnimationChannel::computeRotation( float animationTime, Quaternion4f &result )
{
	if ( getRotationKeys().size() == 1 ) {
		result = getRotationKeys()[ 0 ].value;
		return true;
	}

	unsigned int rotationIndex = 0;
	for ( int i = 0; i < getRotationKeys().size() - 1; i++ ) {
		if ( animationTime < getRotationKeys()[ i + 1 ].time ) {
			rotationIndex = i;
			break;
		}
	}

	auto const &r0 = getRotationKeys()[ rotationIndex ];
	auto const &r1 = getRotationKeys()[ rotationIndex + 1 ];
	float dt = r1.time - r0.time;
	float factor = ( animationTime - r0.time ) / dt;
	Interpolation::slerp( r0.value, r1.value, factor, result );
	return true;
}

bool SkinnedMeshAnimationChannel::computeScale( float animationTime, float &result )
{
	if ( getScaleKeys().size() == 1 ) {
		result = getScaleKeys()[ 0 ].value;
		return true;
	}

	unsigned int scaleIndex = 0;
	for ( int i = 0; i < getScaleKeys().size() - 1; i++ ) {
		if ( animationTime < getScaleKeys()[ i + 1 ].time ) {
			scaleIndex = i;
			break;
		}
	}

	auto const &s0 = getScaleKeys()[ scaleIndex ];
	auto const &s1 = getScaleKeys()[ scaleIndex + 1 ];
	float dt = s1.time - s0.time;
	float factor = ( animationTime - s0.time ) / dt;
	Interpolation::linear( s0.value, s1.value, factor, result );
	return true;
}

SkinnedMeshAnimationClip::SkinnedMeshAnimationClip( void )
{

}

SkinnedMeshAnimationClip::~SkinnedMeshAnimationClip( void )
{

}

SkinnedMeshSkeleton::SkinnedMeshSkeleton( void )
{

}

SkinnedMeshSkeleton::~SkinnedMeshSkeleton( void ) 
{

}

SkinnedMeshAnimationState::SkinnedMeshAnimationState( void )
{

}

SkinnedMeshAnimationState::~SkinnedMeshAnimationState( void )
{

}

SkinnedMesh::SkinnedMesh( void )
	: _animationState( crimild::alloc< SkinnedMeshAnimationState >() )
{

}

SkinnedMesh::~SkinnedMesh( void )
{

}

void SkinnedMesh::debugDump( void )
{
	if ( getSkeleton() == nullptr ) {
		return;
	}
	
	std::cout << "Skinned Mesh: " << std::endl;
	getSkeleton()->getClips().foreach( []( SharedPointer< SkinnedMeshAnimationClip > &clip, unsigned int clipIdx ) {
		std::cout << "  Animation: #" << clipIdx
		  		  << "\n    Duration: " << clip->getDuration()
		  		  << "\n    Frame Rate: " << clip->getFrameRate()
		  		  << "\n    Channels: " << clip->getChannels().size()
		  		  << std::endl;

		clip->getChannels().foreach( []( std::string name, SharedPointer< SkinnedMeshAnimationChannel > &channel, unsigned int channelIdx ) {
			std::cout << "      *" 
					  << " Channel: " << std::string( channel->getName() )
					  << " P=" << channel->getPositionKeys().size() 
					  << "(" << channel->getPositionKeys()[ 0 ].time 
					  << "-" << channel->getPositionKeys()[ channel->getPositionKeys().size() - 1 ].time << ")"
					  << " R=" << channel->getRotationKeys().size()
					  << "(" << channel->getRotationKeys()[ 0 ].time 
					  << "-" << channel->getRotationKeys()[ channel->getRotationKeys().size() - 1 ].time << ")"
					  << " S=" << channel->getScaleKeys().size()
					  << "(" << channel->getScaleKeys()[ 0 ].time 
					  << "-" << channel->getScaleKeys()[ channel->getScaleKeys().size() - 1 ].time << ")"
					  << std::endl;
		});
	});
}
