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

#include "SkinnedMeshComponent.hpp"

#include "Rendering/SkinnedMesh.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/Apply.hpp"
#include "Debug/DebugRenderHelper.hpp"
#include "Foundation/Log.hpp"

CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::SkinnedMeshComponent )

using namespace crimild;

SkinnedMeshComponent::SkinnedMeshComponent( void )
{
	setAnimationParams( 0.0f, -1.0f, true, 1.0f );
}

SkinnedMeshComponent::SkinnedMeshComponent( SharedPointer< SkinnedMesh > const &skinnedMesh )
	: _skinnedMesh( skinnedMesh )
{
	setAnimationParams( 0.0f, -1.0f, true, 1.0f );
}

SkinnedMeshComponent::~SkinnedMeshComponent( void )
{

}

void SkinnedMeshComponent::start( void )
{
	NodeComponent::start();

	_time = 0;
}

void SkinnedMeshComponent::update( const Clock &c )
{
	NodeComponent::update( c );

	_time += c.getDeltaTime();

	auto mesh = getSkinnedMesh();
	if ( mesh == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No skinned mesh attach to component" );
		return;
	}
	
	auto skeleton = mesh->getSkeleton();
	if ( skeleton == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No skinned mesh skeleton attach to component" );
		return;
	}
	
	auto animationState = mesh->getAnimationState();
	if ( animationState == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No skinned mesh animation state attach to component" );
		return;
	}

	animationState->getJointPoses().resize( skeleton->getJoints()->getJointCount() );

	auto currentClip = skeleton->getClips()[ _currentAnimation ];

	float firstFrame = _firstFrame;
	float lastFrame = _lastFrame >= 0.0f ? _lastFrame : currentClip->getDuration();
	bool loop = _loop;

	float timeInTicks = _time * _timeScale * currentClip->getFrameRate();
	float duration = lastFrame - firstFrame;
	float animationTime = firstFrame +  Numericf::clamp( fmod( timeInTicks, duration ), 0.0f, duration );
	float animationProgress = Numericf::min( 1.0f, timeInTicks / ( lastFrame - firstFrame ) );

	if ( !loop && animationProgress >= 1.0f ) {
		animationTime = lastFrame;
	}

	if ( _animationProgressCallback != nullptr ) {
		_animationProgressCallback( animationProgress );
	}

	getNode()->perform( Apply( [mesh, skeleton, currentClip, animationState, animationTime]( Node *node ) {

		Transformation modelTransform;

		if ( currentClip->getChannels().find( node->getName() ) ) {
			auto channel = currentClip->getChannels()[ node->getName() ];

			Transformation tTransform;
			channel->computePosition( animationTime, tTransform.translate() );

			Transformation rTransform;
			channel->computeRotation( animationTime, rTransform.rotate() );

			float scale;
			channel->computeScale( animationTime, scale );
			Transformation sTransform;
			sTransform.setScale( scale );

			modelTransform.computeFrom( rTransform, sTransform );
			modelTransform.computeFrom( tTransform, modelTransform );
		}
		else {
			modelTransform = node->getLocal();
		}
		
		auto joint = skeleton->getJoints()->find( node->getName() );
		if ( joint != nullptr ) {
			Transformation t;
			t.computeFrom( node->getParent()->getWorld(), modelTransform );
			t.computeFrom( t, joint->getOffset() );
			animationState->getJointPoses()[ joint->getId() ] = t.computeModelMatrix();
		}

		node->setLocal( modelTransform );
	}));		
}

void SkinnedMeshComponent::setAnimationParams( 
	float firstFrame, 
	float lastFrame, 
	bool loop, 
	float timeScale,
	float timeOffset,
	SkinnedMeshComponent::AnimationProgressCallback const &callback )
{
	_firstFrame = firstFrame;
	_lastFrame = lastFrame;
	_loop = loop;
	_timeScale = timeScale;
	_time = timeOffset;
	_animationProgressCallback = callback;
}

void SkinnedMeshComponent::renderDebugInfo( Renderer *renderer, Camera *camera )
{
	std::vector< Vector3f > lines;
	auto self = this;
	getNode()->perform( Apply( [&lines, self]( Node *node ) {
		if ( node->hasParent() ) {
			// if ( self->getBones().boneMap.find( node->getName() ) != self->getBones().boneMap.end() ) {
				lines.push_back( node->getParent()->getWorld().getTranslate() );
				lines.push_back( node->getWorld().getTranslate() );
			// }
		}
	}));

	DebugRenderHelper::renderLines( renderer, camera, &lines[ 0 ], lines.size(), RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) );
}

bool SkinnedMeshComponent::registerInStream( Stream &s )
{
	if ( !NodeComponent::registerInStream( s ) ) {
		return false;
	}

	if ( _skinnedMesh != nullptr ) {
		_skinnedMesh->registerInStream( s );
	}

	return true;
}

void SkinnedMeshComponent::save( Stream &s )
{
	NodeComponent::save( s );

	s.write( _skinnedMesh );
}

void SkinnedMeshComponent::load( Stream &s )
{
	NodeComponent::load( s );

	s.read( _skinnedMesh );
}

