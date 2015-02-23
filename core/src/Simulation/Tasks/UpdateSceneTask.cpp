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

#include "UpdateSceneTask.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/UpdateComponents.hpp"
#include "Visitors/UpdateWorldState.hpp"
#include "SceneGraph/Node.hpp"

#include <thread>
#include <chrono>

using namespace crimild;

#define CRIMILD_SIMULATION_TIME 1.0 / 60.0

UpdateSceneTask::UpdateSceneTask( int priority )
	: Task( priority ),
	  _accumulator( 0.0 )
{

}

UpdateSceneTask::~UpdateSceneTask( void )
{

}

void UpdateSceneTask::start( void )
{
	_accumulator = 0.0;
}

void UpdateSceneTask::update( void )
{
    CRIMILD_PROFILE( "UpdateSceneTask - Update" )
    
    const Time &t = Simulation::getInstance().getSimulationTime();
    
    _accumulator += t.getDeltaTime();
    
    auto scene = Simulation::getInstance().getScene();
    if ( scene == nullptr ) {
        return;
    }
    
    Time fixed = t;
    fixed.setDeltaTime( CRIMILD_SIMULATION_TIME );
    while ( _accumulator >= CRIMILD_SIMULATION_TIME ) {
        scene->perform( UpdateComponents( fixed ) );
        _accumulator -= CRIMILD_SIMULATION_TIME;
    }
}

void UpdateSceneTask::stop( void )
{

}

