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

#include "ParticleSystemComponent.hpp"
#include "MaterialComponent.hpp"
#include "Foundation/Log.hpp"
#include "SceneGraph/Geometry.hpp"

using namespace crimild;

ParticleSystemComponent::ParticleSystemComponent( void )
    : _primitive( crimild::alloc< Primitive >( Primitive::Type::POINTS ) ),
	  _material( crimild::alloc< Material >() ),
 	  _particleCount( 50 ),
	  _particleSize( 20.0f ),
	  _particleDuration( 1.0f ),
	  _gravity( 0.0f, -9.8f, 0.0f ),
      _velocity( 1.0f, 1.0f, 1.0f ),
	  _spread( 1.0f, 1.0f, 1.0f ),
	  _looping( true ),
	  _gravityUniform( crimild::alloc< Vector3fUniform >( "uGravity", Vector3f( 0.0f, 0.0f, 0.0f ) ) ),
	  _timeUniform( crimild::alloc< FloatUniform >( "uTime", 0.0f ) ),
	  _durationUniform( crimild::alloc< FloatUniform >( "uLifeTime", 1.0f ) ),
	  _shapeRadiusUniform( crimild::alloc< FloatUniform >( "uShape.radius", 0.0f ) ),
	  _shapeCenterUniform( crimild::alloc< Vector3fUniform >( "uShape.center", Vector3f( 0.0f, 0.0f, 0.0f ) ) )
{
	_material->getAlphaState()->setEnabled( true );
}

ParticleSystemComponent::~ParticleSystemComponent( void )
{

}

void ParticleSystemComponent::onAttach( void )
{
    auto geometry = getNode< Geometry >();
	if ( geometry == nullptr ) {
		Log::Error << "Cannot attach a particle system to a node that is not a Geometry" << Log::End;
		exit( 1 );
	}

	auto program = _material->getProgram();
	if ( program == nullptr ) {
		Log::Error << "Particle system component requires a valid shader program in order to work" << Log::End;
		exit( 1 );
	}

	program->attachUniform( _gravityUniform );
	program->attachUniform( _timeUniform );
	program->attachUniform( _durationUniform );
	program->attachUniform( _shapeRadiusUniform );
	program->attachUniform( _shapeCenterUniform );

	generateParticles();
	geometry->attachPrimitive( _primitive );
	geometry->getComponent< MaterialComponent >()->attachMaterial( _material );
}

void ParticleSystemComponent::update( const Clock &t )
{
	_durationUniform->setValue( _particleDuration );
	_gravityUniform->setValue( _gravity );
	_timeUniform->setValue( ( float ) 0.5f * t.getCurrentTime() );
	if ( _shape != nullptr ) {
		_shapeRadiusUniform->setValue( _shape->getRadius() );
		_shapeCenterUniform->setValue( _shape->getCenter() );
	}
}

void ParticleSystemComponent::generateParticles( void )
{
	int particleCount = getParticleCount();

	VertexFormat format = VertexFormat::VF_P3_N3_UV2;
    auto vbo = crimild::alloc< VertexBufferObject >( format, particleCount, nullptr );
	float *vertices = vbo->getData();

    auto ibo = crimild::alloc< IndexBufferObject >( particleCount, nullptr );
	unsigned short *indices = ibo->getData();

	for ( unsigned short i = 0; i < particleCount; i++ ) {
		vertices[ i * format.getVertexSize() + 0 ] = getSpread()[ 0 ] * ( 2.0f * Numericf::random() - 1.0f );
		vertices[ i * format.getVertexSize() + 1 ] = getSpread()[ 1 ] * ( 2.0f * Numericf::random() - 1.0f );
		vertices[ i * format.getVertexSize() + 2 ] = getSpread()[ 2 ] * ( 2.0f * Numericf::random() - 1.0f );

		vertices[ i * format.getVertexSize() + 3 ] = getVelocity()[ 0 ];
		vertices[ i * format.getVertexSize() + 4 ] = getVelocity()[ 1 ];
		vertices[ i * format.getVertexSize() + 5 ] = getVelocity()[ 2 ];

		vertices[ i * format.getVertexSize() + 6 ] = getParticleSize();
		vertices[ i * format.getVertexSize() + 7 ] = Numericf::random( getParticleDuration() );

		indices[ i ] = i;
	}

	_primitive->setVertexBuffer( vbo );
	_primitive->setIndexBuffer( ibo );
}

