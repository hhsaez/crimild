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

#include "UpdateSceneAndPhysicsTask.hpp"

#if CRIMILD_ENABLE_PHYSICS
#include <Crimild_Physics.hpp>
#endif

using namespace crimild;

#define CRIMILD_SIMULATION_TIME 1.0 / 60.0

UpdateSceneAndPhysicsTask::UpdateSceneAndPhysicsTask( int priority )
	: Task( priority ),
	  _dtAccumulator( 0.0f )
{
}

UpdateSceneAndPhysicsTask::~UpdateSceneAndPhysicsTask( void )
{

}

void UpdateSceneAndPhysicsTask::start( void )
{
    _dtAccumulator = 0.0;
    
#if CRIMILD_ENABLE_PHYSICS
    float gx = Simulation::getInstance()->getSettings().get( "physics.gravity.x", 0.0f );
    float gy = Simulation::getInstance()->getSettings().get( "physics.gravity.y", -9.8f );
    float gz = Simulation::getInstance()->getSettings().get( "physics.gravity.z", 0.0f );
    physics::PhysicsContext::getInstance().setGravity( Vector3f( gx, gy, gz ) );
#endif
}

void UpdateSceneAndPhysicsTask::update( void )
{
    CRIMILD_PROFILE( "Update Scene and Physics" )
    
    const Time &t = Simulation::getInstance()->getSimulationTime();
    
    _dtAccumulator += t.getDeltaTime();
    
    auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
        return;
    }
    
    Time fixed = t;
    fixed.setDeltaTime( CRIMILD_SIMULATION_TIME );
    while ( _dtAccumulator >= CRIMILD_SIMULATION_TIME ) {
#if CRIMILD_ENABLE_PHYSICS
        physics::PhysicsContext::getInstance().step( CRIMILD_SIMULATION_TIME );
#endif
        scene->perform( UpdateComponents( fixed ) );
        _dtAccumulator -= CRIMILD_SIMULATION_TIME;
    }
}

void UpdateSceneAndPhysicsTask::stop( void )
{

}

