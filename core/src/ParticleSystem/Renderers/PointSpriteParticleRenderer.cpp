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

#include "PointSpriteParticleRenderer.hpp"
#include "Simulation/AssetManager.hpp"
#include "Rendering/Renderer.hpp"
#include "Components/MaterialComponent.hpp"
#include "Concurrency/Async.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"

using namespace crimild;

PointSpriteParticleRenderer::PointSpriteParticleRenderer( void )
{
	// create the material here so it can be modified later
	_material = crimild::alloc< Material >();

    //_material->setProgram( AssetManager::getInstance()->get< PointSpriteShaderProgram >() );
}

PointSpriteParticleRenderer::~PointSpriteParticleRenderer( void )
{

}

void PointSpriteParticleRenderer::configure( Node *node, ParticleData *particles )
{
	_geometry = crimild::alloc< Geometry >();
	if ( _material != nullptr ) {
		_geometry->getComponent< MaterialComponent >()->attachMaterial( _material );
	}

	static_cast< Group * >( node )->attachNode( _geometry );

	_positions = particles->getAttrib( ParticleAttrib::POSITION );
	_colors = particles->getAttrib( ParticleAttrib::COLOR );
	_sizes = particles->getAttrib( ParticleAttrib::UNIFORM_SCALE );

    _primitive = crimild::alloc< Primitive >( Primitive::Type::POINTS );

	_geometry->attachPrimitive( _primitive );
}

void PointSpriteParticleRenderer::update( Node *node, crimild::Real64 dt, ParticleData *particles )
{
    assert( false );
    /*
    const auto pCount = particles->getAliveCount();
    if ( pCount == 0 ) {
        return;
    }

    auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3_C4_UV2, pCount );

	const auto ps = _positions->getData< Vector3f >();
	const auto ss = _sizes->getData< crimild::Real32 >();
	const auto cs = _colors->getData< RGBAColorf >();

    // TODO: should I traverse the arrays by grouping data in 4/8 byte touples?

	if ( particles->shouldComputeInWorldSpace() ) {
		for ( auto i = 0; i < pCount; i++ ) {
			auto p = ps[ i ];
			// TODO: cache inverse transform?
			node->getWorld().applyInverseToPoint( p, p );
			vbo->setPositionAt( i, p );
		}
	}
	else {
		for ( auto i = 0; i < pCount; i++ ) {
			vbo->setPositionAt( i, ps[ i ] );
		}
	}

	for ( auto i = 0; i < pCount; i++ ) {
		vbo->setTextureCoordAt( i, Vector2f( ss[ i ], 0.0f ) );
	}

	for ( auto i = 0; i < pCount; i++ ) {
		vbo->setRGBAColorAt( i, cs[ i ] );
	}

    auto ibo = crimild::alloc< IndexBufferObject >( pCount );
    ibo->generateIncrementalIndices();

    crimild::concurrency::sync_frame( [this, vbo, ibo] {
        _primitive->setVertexBuffer( vbo );
        _primitive->setIndexBuffer( ibo );
    });
    */
}

void PointSpriteParticleRenderer::encode( coding::Encoder &encoder )
{
	ParticleSystemComponent::ParticleRenderer::encode( encoder );

	encoder.encode( "material", _material );
}

void PointSpriteParticleRenderer::decode( coding::Decoder &decoder )
{
	ParticleSystemComponent::ParticleRenderer::decode( decoder );

	decoder.decode( "material", _material );

	if ( _material == nullptr ) {
		_material = crimild::alloc< Material >();
	}

    //_material->setProgram( AssetManager::getInstance()->get< PointSpriteShaderProgram >() );

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
