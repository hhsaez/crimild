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

#include "AnimatedSpriteParticleRenderer.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Components/MaterialComponent.hpp"
#include "Concurrency/Async.hpp"
#include "Rendering/Renderer.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;

AnimatedSpriteParticleRenderer::AnimatedSpriteParticleRenderer( void )
    : _spriteSheetSize { 4.0f, 4.0f }
{
    // create the material here so it can be modified later
    _material = crimild::alloc< Material >();

    //_material->setProgram( crimild::alloc< UnlitShaderProgram >() );
}

AnimatedSpriteParticleRenderer::~AnimatedSpriteParticleRenderer( void )
{
}

void AnimatedSpriteParticleRenderer::configure( Node *node, ParticleData *particles )
{
    _geometry = crimild::alloc< Geometry >();
    if ( _material != nullptr ) {
        _geometry->getComponent< MaterialComponent >()->attachMaterial( _material );
    }

    static_cast< Group * >( node )->attachNode( _geometry );

    _positions = particles->getAttrib( ParticleAttrib::POSITION );
    _sizes = particles->getAttrib( ParticleAttrib::UNIFORM_SCALE );
    _times = particles->getAttrib( ParticleAttrib::TIME );
    _lifetimes = particles->getAttrib( ParticleAttrib::LIFE_TIME );

    _primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );

    _geometry->attachPrimitive( _primitive );
}

void AnimatedSpriteParticleRenderer::update( Node *node, crimild::Real64 dt, ParticleData *particles )
{
    /*
    const auto pCount = particles->getAliveCount();
    if ( pCount == 0 ) {
        return;
    }

    auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3_UV2, 4 * pCount );
	auto ibo = crimild::alloc< IndexBufferObject >( 6 * pCount );

	auto up = Vector3f::UNIT_Y;
	auto right = Vector3f::UNIT_X;

	if ( shouldUseOrientedQuads() ) {
		const auto camera = Camera::getMainCamera();
		up = camera->getWorld().computeUp();
		right = camera->getWorld().computeRight();

		node->getWorld().applyInverseToVector( up, up );
		node->getWorld().applyInverseToVector( right, right );
	}

	const auto offset0 = up - right;
	const auto offset1 = -up - right;
	const auto offset2 = -up + right;
	const auto offset3 = up + right;

	const crimild::UInt8 frameCount = _spriteSheetSize.x() * _spriteSheetSize.y();
	const auto spriteSize = Vector2f( 1.0f / _spriteSheetSize.x(), 1.0f / _spriteSheetSize.y() );

	const auto uv0 = Vector2f( 0.0f, 0.0f );
	const auto uv1 = Vector2f( 0.0f, spriteSize.y() );
	const auto uv2 = Vector2f( spriteSize.x(), spriteSize.y() );
	const auto uv3 = Vector2f( spriteSize.x(), 0.0f );

	const auto ps = _positions->getData< Vector3f >();
	const auto ss = _sizes->getData< crimild::Real32 >();
	const auto timeData = _times->getData< crimild::Real32 >();
	const auto lifetimeData = _lifetimes->getData< crimild::Real32 >();

	// Vertex data is interleaved, so it should be more efficient
	// to set each vertex attribute per loop (as below) instead of
	// using separated loops (as in the case of other updaters/renderers)
	// TODO: I need to confirm this somehow
	for ( auto i = 0; i < pCount; i++ ) {
		auto idx = i * 4;
		auto pos = ps[ i ];
		if ( particles->shouldComputeInWorldSpace() ) {
			// TODO: cache inverse transform?
			node->getWorld().applyInverseToPoint( pos, pos );
		}
		auto s = ss[ i ];

		const auto t = 1.0f - ( timeData[ i ] / lifetimeData[ i ] );
		const auto frame = ( crimild::UInt8 )( frameCount * t );
		const auto fx = frame % ( ( crimild::UInt8 ) _spriteSheetSize.x() );
		const auto fy = frame / ( ( crimild::UInt8 ) _spriteSheetSize.y() );
		const auto frameOffset = Vector2f( fx * spriteSize.x(), fy * spriteSize.y() );

		vbo->setPositionAt( idx + 0, pos + s * offset0 );
		vbo->setTextureCoordAt( idx + 0, frameOffset + uv0 );

		vbo->setPositionAt( idx + 1, pos + s * offset1 );
		vbo->setTextureCoordAt( idx + 1, frameOffset + uv1 );

		vbo->setPositionAt( idx + 2, pos + s * offset2 );
		vbo->setTextureCoordAt( idx + 2, frameOffset + uv2 );

		vbo->setPositionAt( idx + 3, pos + s * offset3 );
		vbo->setTextureCoordAt( idx + 3, frameOffset + uv3 );
	}

	for ( auto i = 0; i < pCount; i++ ) {
		const auto idx = i * 6;
		const auto vdx = i * 4;
		ibo->setIndexAt( idx + 0, vdx + 0 );
		ibo->setIndexAt( idx + 1, vdx + 1 );
		ibo->setIndexAt( idx + 2, vdx + 2 );
		ibo->setIndexAt( idx + 3, vdx + 0 );
		ibo->setIndexAt( idx + 4, vdx + 2 );
		ibo->setIndexAt( idx + 5, vdx + 3 );
	}

    crimild::concurrency::sync_frame( [this, vbo, ibo] {
        _primitive->setVertexBuffer( vbo );
        _primitive->setIndexBuffer( ibo );
    });
    */
}

void AnimatedSpriteParticleRenderer::encode( coding::Encoder &encoder )
{
    ParticleSystemComponent::ParticleRenderer::encode( encoder );

    encoder.encode( "material", _material );
    encoder.encode( "spriteSheetSize", _spriteSheetSize );
    encoder.encode( "useOrientedQuads", _useOrientedQuads );
}

void AnimatedSpriteParticleRenderer::decode( coding::Decoder &decoder )
{
    ParticleSystemComponent::ParticleRenderer::decode( decoder );

    decoder.decode( "material", _material );
    decoder.decode( "spriteSheetSize", _spriteSheetSize );
    decoder.decode( "useOrientedQuads", _useOrientedQuads );

    if ( _material == nullptr ) {
        _material = crimild::alloc< Material >();
    }

    //_material->setProgram( crimild::alloc< UnlitShaderProgram >() );

    /*
    std::string blendMode;
    decoder.decode( "blendMode", blendMode );
    if ( blendMode == "additive" ) {
        _material->setAlphaState( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::SRC_ALPHA, AlphaState::DstBlendFunc::ONE ) );
    }
    else if ( blendMode == "color" ) {
        _material->setAlphaState( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::SRC_COLOR, AlphaState::DstBlendFunc::ONE_MINUS_SRC_COLOR ) );
    }
    else if ( blendMode == "transparent" ) {
        _material->setAlphaState( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::SRC_ALPHA, AlphaState::DstBlendFunc::ONE_MINUS_SRC_ALPHA ) );
    }
    else if ( blendMode == "additive_no_alpha" ) {
        _material->setAlphaState( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::ONE, AlphaState::DstBlendFunc::ONE ) );
    }
    else if ( blendMode == "multiply" ) {
        _material->setAlphaState( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::ONE, AlphaState::DstBlendFunc::ONE_MINUS_SRC_ALPHA ) );
    }
    else if ( blendMode == "default" ) {
        _material->setAlphaState( AlphaState::ENABLED );
    }
    else {
        _material->setAlphaState( AlphaState::DISABLED );
    }

    crimild::Bool cullFaceEnabled = true;
    decoder.decode( "cullFaceEnabled", cullFaceEnabled );
    _material->getCullFaceState()->setEnabled( cullFaceEnabled );

    crimild::Bool depthStateEnabled = true;
    decoder.decode( "depthStateEnabled", depthStateEnabled );
    _material->setDepthState( depthStateEnabled ? DepthState::ENABLED : DepthState::DISABLED );
    */
}
