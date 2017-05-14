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

#include "LuaParticleSystemComponentBuilder.hpp"

#include "SceneGraph/LuaSceneBuilder.hpp"

using namespace crimild;
using namespace crimild::scripting;

SharedPointer< ParticleSystemComponent > LuaParticleSystemComponentBuilder::build( ScriptEvaluator &eval )
{
	crimild::UInt16 maxParticles;
	if ( !eval.getPropValue( "maxParticles", maxParticles ) || maxParticles == 0 ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "No 'maxParticles' member found" );
		return nullptr;
	}

	auto particles = crimild::alloc< ParticleData >( maxParticles );

	crimild::Bool computeInWorldSpace = false;
	if ( eval.getPropValue( "computeInWorldSpace", computeInWorldSpace ) ) {
		particles->setComputeInWorldSpace( computeInWorldSpace );
	}

	auto ps = crimild::alloc< ParticleSystemComponent >( particles );

	crimild::Real32 emitRate = maxParticles;
	if ( eval.getPropValue( "emitRate", emitRate ) ) {
		ps->setEmitRate( emitRate );
	}

	crimild::Real32 preWarmTime = 0.0;
	if ( eval.getPropValue( "preWarmTime", preWarmTime ) ) {
		ps->setPreWarmTime( preWarmTime );
	}

	crimild::Bool burst = false;
	if ( eval.getPropValue( "burst", burst ) ) {
		ps->setBurst( burst );
	}
	
	eval.foreach( "generators", [ ps ]( ScriptEvaluator &gEval, int ) {
		std::string type;
		if ( gEval.getPropValue( "type", type ) ) {
			auto builder = LuaObjectBuilderRegistry::getInstance()->getBuilder( type );
			if ( builder != nullptr ) {
				auto generator = crimild::cast_ptr< ParticleSystemComponent::ParticleGenerator >( builder( gEval ) );
				if ( generator != nullptr ) {
					ps->addGenerator( generator );
				}
				else {
					Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot build generator of type ", type );
				}
			}
			else {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "No builder defined for type ", type );
			}
		}
		else {
			Log::error( CRIMILD_CURRENT_CLASS_NAME, "No generator type provided" );
		}
	});
	
	eval.foreach( "updaters", [ ps ]( ScriptEvaluator &gEval, int ) {
		std::string type;
		if ( gEval.getPropValue( "type", type ) ) {
			auto builder = LuaObjectBuilderRegistry::getInstance()->getBuilder( type );
			if ( builder != nullptr ) {
				auto updater = crimild::cast_ptr< ParticleSystemComponent::ParticleUpdater >( builder( gEval ) );
				if ( updater != nullptr ) {
					ps->addUpdater( updater );
				}
				else {
					Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot build updater of type ", type );
				}
			}
			else {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "No builder defined for type ", type );
			}
		}
		else {
			Log::error( CRIMILD_CURRENT_CLASS_NAME, "No updater type provided" );
		}
	});
	
	eval.foreach( "renderers", [ ps ]( ScriptEvaluator &gEval, int ) {
		std::string type;
		if ( gEval.getPropValue( "type", type ) ) {
			auto builder = LuaObjectBuilderRegistry::getInstance()->getBuilder( type );
			if ( builder != nullptr ) {
				auto renderer = crimild::cast_ptr< ParticleSystemComponent::ParticleRenderer >( builder( gEval ) );
				if ( renderer != nullptr ) {
					ps->addRenderer( renderer );
				}
				else {
					Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot build renderer of type ", type );
				}
			}
			else {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "No builder defined for type ", type );
			}
		}
		else {
			Log::error( CRIMILD_CURRENT_CLASS_NAME, "No renderer type provided" );
		}
	});
	
	return ps;
}

