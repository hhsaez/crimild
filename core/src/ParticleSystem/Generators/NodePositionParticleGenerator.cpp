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

#include "NodePositionParticleGenerator.hpp"

#include "Mathematics/Random.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;

NodePositionParticleGenerator::NodePositionParticleGenerator( void )
{

}

NodePositionParticleGenerator::~NodePositionParticleGenerator( void )
{

}

void NodePositionParticleGenerator::configure( Node *node, ParticleData *particles )
{
	_positions = particles->createAttribArray< Vector3f >( ParticleAttrib::POSITION );
}

void NodePositionParticleGenerator::generate( Node *node, crimild::Real64 dt, ParticleData *particles, ParticleId startId, ParticleId endId )
{
	assert( _targetNode != nullptr );

	auto ps = _positions->getData< Vector3f >();

	auto origin = _targetNode->getWorld().getTranslate();
	node->getWorld().applyInverseToPoint( origin, origin );
	
    const auto posMin = origin - _size;
    const auto posMax = origin + _size;

	// TODO: use random vectors
    for ( ParticleId i = startId; i < endId; i++ ) {
        auto x = Random::generate< Real32 >( posMin.x(), posMax.x() );
        auto y = Random::generate< Real32 >( posMin.y(), posMax.y() );
        auto z = Random::generate< Real32 >( posMin.z(), posMax.z() );
		if ( particles->shouldComputeInWorldSpace() ) {
			auto p = Vector3f( x, y, z );
			node->getWorld().applyToPoint( p, p );
			ps[ i ] = p;
		}
		else {
			ps[ i ] = Vector3f( x, y, z );
		}
    }
}

