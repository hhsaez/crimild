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

#include "UpdateTimeTask.hpp"

#include "Rendering/GL3/Utils.hpp"

#include <thread>
#include <chrono>

using namespace crimild;

#if 0

UpdateTimeTask::UpdateTimeTask( int priority )
	: Task( priority )
{
	 CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( UpdateTimeTask::Messages::ResetSimulationTime, UpdateTimeTask, onResetSimulationTime );
	 CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( LoadSceneTask::Messages::SceneLoaded, UpdateTimeTask, onSceneLoaded );

	setRepeatMode( Task::RepeatMode::REPEAT );
	setThreadMode( Task::ThreadMode::BACKGROUND );
	setSyncMode( Task::SyncMode::FRAME );
}

UpdateTimeTask::~UpdateTimeTask( void )
{

}

void UpdateTimeTask::run( void )
{
	CRIMILD_PROFILE( "Update Time" )
	
	Time &t = Simulation::getInstance()->getSimulationTime();
	double currentTime = glfwGetTime();
	t.update( currentTime );
}

void UpdateTimeTask::start( void )
{
	resetTime();
}

void UpdateTimeTask::stop( void )
{

}

void UpdateTimeTask::update( void )
{

}

void UpdateTimeTask::onResetSimulationTime( Messages::ResetSimulationTime const & )
{
	resetTime();
}

void UpdateTimeTask::onSceneLoaded( LoadSceneTask::Messages::SceneLoaded const & )
{
	resetTime();
}

void UpdateTimeTask::resetTime( void )
{
	Time &t = Simulation::getInstance()->getSimulationTime();
	double currentTime = glfwGetTime();
	t.reset( currentTime );
}

#endif

