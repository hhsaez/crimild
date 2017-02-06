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

#include "ParticleSystem.hpp"

#include "Primitives/Primitive.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Material.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/LambdaComponent.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/AssetManager.hpp"
#include "Mathematics/Interpolation.hpp"
#include "Mathematics/Random.hpp"
#include "Concurrency/Async.hpp"

using namespace crimild;

void ParticleEmitter::generate( Vector3f &position, Vector3f &velocity ) const
{
    evaluate( position, velocity );
    _transformation.applyToPoint( position, position );
    _transformation.applyToVector( velocity, velocity );
    velocity.normalize();
};

void PointParticleEmitter::evaluate( Vector3f &position, Vector3f &velocity ) const
{
    position = Vector3f( 0.0f, 0.0f, 0.0f );
    velocity = Vector3f( 0.0f, 0.0f, 0.0f );
}

void ConeParticleEmitter::evaluate( Vector3f &position, Vector3f &velocity ) const
{
    float u = Random::generate< float >() * _height;
    float t = Random::generate< float >() * Numericf::TWO_PI;
    
    position[ 0 ] = ( ( _height - u ) / _height ) * _radius * Numericf::cos( t );
    position[ 2 ] = ( ( _height - u ) / _height ) * _radius * Numericf::sin( t );
    position[ 1 ] = u;

    Vector3f p1;
    p1[ 0 ] = _radius * Numericf::cos( t );
    p1[ 2 ] = _radius * Numericf::sin( t );
    p1[ 1 ] = 0.0f;

    velocity = p1 - position;
}

void CylinderParticleEmitter::evaluate( Vector3f &position, Vector3f &velocity ) const
{
    float u = Random::generate< float >() * _height;
    float t = Random::generate< float >() * Numericf::TWO_PI;
    float r = Random::generate< float >( 0.1f, 1.0f ) * _radius;
    
    position[ 0 ] = r * Numericf::cos( t );
    position[ 2 ] = r * Numericf::sin( t );
    position[ 1 ] = u;

    velocity = Vector3f( 0.0f, 1.0f, 0.0f );
}

void SphereParticleEmitter::evaluate( Vector3f &position, Vector3f &velocity ) const
{
    float s = Random::generate< float >() * Numericf::PI;
    float t = Random::generate< float >() * Numericf::TWO_PI;
    
    position[ 0 ] = _radius * Numericf::cos( t ) * Numericf::sin( s );
    position[ 2 ] = _radius * Numericf::sin( t ) * Numericf::sin( s );
    position[ 1 ] = _radius * Numericf::cos( s );

    velocity = position;
}

ParticleSystem::ParticleSystem( void )
    : _particleStartColor( 1.0f, 1.0f, 1.0f, 1.0f ),
      _particleEndColor( 1.0f, 1.0f, 1.0f, 1.0f )
{
    auto self = this;
    attachComponent< LambdaComponent >( [self]( Node *, const Clock &c ) {
        self->updateParticles( c );
    });

    setEmitter( crimild::alloc< PointParticleEmitter >() );
}

ParticleSystem::~ParticleSystem( void )
{

}

void ParticleSystem::generate( void )
{
    auto material = crimild::alloc< Material >();
    getComponent< MaterialComponent >()->detachAllMaterials();
    getComponent< MaterialComponent >()->attachMaterial( material );

    material->setColorMap( _texture );
    material->setAlphaState( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::SRC_ALPHA, AlphaState::DstBlendFunc::ONE ) );
    material->setCullFaceState( CullFaceState::DISABLED );

    _program = crimild::retain( AssetManager::getInstance()->get< ShaderProgram >( Renderer::SHADER_PROGRAM_PARTICLE_SYSTEM ) );
    material->setProgram( _program );

    _primitive = crimild::alloc< Primitive >( Primitive::Type::POINTS );

    _particles.resize( getMaxParticles() );
    for ( auto &p : _particles ) {
        resetParticle( p );
        if ( shouldPreComputeParticles() ) {
            p.time = Random::generate< float >( 0.0f, 1.0f );
            p.position += p.time * getParticleSpeed() * getParticleLifetime() * p.velocity;

            //}
            
        }
    }

    auto ibo = crimild::alloc< IndexBufferObject >( getMaxParticles() );
    for ( IndexPrecision i = 0; i < getMaxParticles(); i++ ) {
        ibo->setIndexAt( i, i );
    }
    _primitive->setIndexBuffer( ibo );

    attachPrimitive( _primitive );
}

void ParticleSystem::resetParticle( ParticleSystem::Particle &p )
{
    p.time = 0.0f;
    getEmitter()->generate( p.position, p.velocity );

    if ( useWorldSpace() ) {
        Node *parent = this;
        if ( hasParent() ) {
            parent = getParent();
        }

        parent->getWorld().applyToPoint( p.position, p.position );
        parent->getWorld().applyToVector( p.velocity, p.velocity );
    }
}

void ParticleSystem::updateParticles( const Clock &c )
{
    auto camera = Simulation::getInstance()->getMainCamera();

    float timeModifier = 1.0f / getParticleLifetime();

    for ( auto &p : _particles ) {
        p.time += timeModifier * c.getDeltaTime();
        if ( p.time >= 1.0f ) {
            resetParticle( p );
        }

        p.position += c.getDeltaTime() * getParticleSpeed() * p.velocity;
    }

    Vector3f localCameraPos;
    getWorld().applyInverseToPoint( camera->getWorld().getTranslate(), localCameraPos );

    std::sort( _particles.begin(), _particles.end(), [localCameraPos]( const ParticleSystem::Particle &a, const ParticleSystem::Particle &b ) -> bool {
        return a.position[ 2 ] < b.position[ 2 ];
    });

    auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3_C4_UV2, getMaxParticles() );
    for ( unsigned int i = 0; i < getMaxParticles(); i++ ) {
        RGBAColorf color;
        Interpolation::linear( getParticleStartColor(), getParticleEndColor(), _particles[ i ].time, color );

        float size;
        Interpolation::linear( getParticleStartSize(), getParticleEndSize(), _particles[ i ].time, size );

        Vector3f pos;
        getParent()->getWorld().applyInverseToPoint( _particles[ i ].position, pos );

        vbo->setPositionAt( i, pos );
        vbo->setTextureCoordAt( i, Vector2f( size, 0.0f ) );
        vbo->setRGBAColorAt( i, color );
    }

    crimild::concurrency::sync_frame( [this, vbo] {
        _primitive->setVertexBuffer( vbo );
    });
}

