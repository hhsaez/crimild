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

#include "InstancedParticleRenderer.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Programs/UnlitShaderProgram.hpp"
#include "Rendering/Programs/ForwardShadingShaderProgram.hpp"
#include "Components/MaterialComponent.hpp"
#include "Simulation/AssetManager.hpp"
#include "SceneGraph/Camera.hpp"
#include "Concurrency/Async.hpp"
#include "Visitors/ApplyToGeometries.hpp"

using namespace crimild;

void InstancedParticleRenderer::configure( Node *node, ParticleData *particles ) 
{
	auto count = particles->getParticleCount();
	_modelBO = crimild::alloc< Matrix4fInstancedBufferObject >( count, nullptr );
	_modelBO->setUsage( ResourceUsage::Dynamic );
	
	auto modelBO = _modelBO;
	auto lit = true;
	node->perform(
		ApplyToGeometries(
			[ modelBO, lit ]( Geometry *g ) {
				g->forEachPrimitive(
					[ modelBO ]( Primitive *p ) {
						p->setInstancedBuffer( modelBO );
					}
				);
				g->getComponent< MaterialComponent >()->forEachMaterial(
					[ lit ]( Material *m ) {
						if ( lit ) {
							m->setProgram( crimild::alloc< ForwardShadingShaderProgram >( true ) );
						}
						else {
							m->setProgram( crimild::alloc< UnlitShaderProgram >( true ) );
						}
						m->setCastShadows( true );
					}
				);
				g->setCullMode( Node::CullMode::NEVER );
				g->setInstancingEnabled( true );
			}
		)
	);
}

void InstancedParticleRenderer::update( Node *node, crimild::Real64 dt, ParticleData *particles )
{
	// todo: update model BO
	auto pCount = particles->getAliveCount();
	if ( pCount == 0 ) {
		return;
	}
	
	auto ps = particles->getAttrib( ParticleAttrib::POSITION )->getData< Vector3f >();
	auto ss = particles->getAttrib( ParticleAttrib::UNIFORM_SCALE )->getData< crimild::Real32 >();
	auto as = particles->getAttrib( ParticleAttrib::EULER_ANGLES )->getData< Vector3f >();
	auto msCount = _modelBO->getInstanceCount();
	auto ms = _modelBO->values();
    
	crimild::Size i = 0;
    
	for ( ; i < pCount; ++i ) {
		Transformation t;
        
		t.setTranslate( ps[ i ] );
		t.setScale( ss[ i ] );
		t.setRotate( Quaternion4f::createFromEulerAngles( as[ i ] ) );
        
		ms[ i ] = t.computeModelMatrix();
	}
    
	const auto inf = [] {
		Transformation t;
		t.setTranslate( Vector3f::NEGATIVE_INFINITY );
		return t.computeModelMatrix();
	}();
    
	for ( ; i < msCount; ++i ) {
		ms[ i ] = inf;
	}            
}

