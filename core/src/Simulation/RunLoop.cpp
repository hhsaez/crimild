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

#include "RunLoop.hpp"

#include <iostream>
#include <algorithm>

using namespace crimild;

RunLoop::RunLoop( void )
{

}

RunLoop::~RunLoop( void )
{
	if ( hasActiveTasks() ) {
		stop();
	}

	cleanup();
}

void RunLoop::startTask( Task *task )
{
	if ( isTaskActive( task ) || isTaskKilled( task ) || isTaskSuspended( task ) ) {
		return;
	}

	auto it = _activeTasks.begin();
	for ( ; it != _activeTasks.end(); it++ ) {
		if ( ( *it )->getPriority() > task->getPriority() ) {
			break;
		}
	}
    Pointer< Task > taskPtr( task );
	_activeTasks.insert( it, taskPtr );
	task->start();
}

void RunLoop::stopTask( Task *task )
{
	if ( isTaskKilled( task ) ) {
		return;
	}

    Pointer< Task > taskPtr( task );
	_killedTasks.push_back( taskPtr );

	if ( isTaskActive( task ) ) {
		_activeTasks.remove( taskPtr );
	}
	else {
		_suspendedTasks.remove( taskPtr );
	}
}

void RunLoop::suspendTask( Task *task )
{
	if ( isTaskActive( task ) ) {
        Pointer< Task > taskPtr( task );
		_suspendedTasks.push_back( taskPtr );
		_activeTasks.remove( taskPtr );
		task->suspend();
	}
}

void RunLoop::resumeTask( Task *task )
{
	if ( !isTaskSuspended( task ) ) {
		return;
	}

	auto it = _activeTasks.begin();
	for ( ; it != _activeTasks.end(); it++ ) {
		if ( ( *it )->getPriority() > task->getPriority() ) {
			break;
		}
	}
    Pointer< Task > taskPtr( task );
	_activeTasks.insert( it, taskPtr );
	_suspendedTasks.remove( taskPtr );

	task->resume();
}

bool RunLoop::isTaskActive( Task *task ) const
{
	return std::find( std::begin( _activeTasks ), std::end( _activeTasks ), task ) != std::end( _activeTasks );
}

void RunLoop::foreachActiveTask( std::function< void ( Task *task ) > callback )
{
	for ( auto task : _activeTasks ) {
		callback( task.get() );
	}
}

bool RunLoop::isTaskKilled( Task *task ) const
{
	return std::find( std::begin( _killedTasks ), std::end( _killedTasks ), task ) != std::end( _killedTasks );
}

void RunLoop::foreachKilledTask( std::function< void ( Task *task ) > callback )
{
	for ( auto task : _killedTasks ) {
		callback( task.get() );
	}
}

bool RunLoop::isTaskSuspended( Task *task ) const
{
	return std::find( std::begin( _suspendedTasks ), std::end( _suspendedTasks ), task ) != std::end( _suspendedTasks );
}

void RunLoop::foreachSuspendedTask( std::function< void ( Task *task ) > callback )
{
	for ( auto task : _suspendedTasks ) {
		callback( task.get() );
	}
}

bool RunLoop::update( void )
{
	auto it = _activeTasks.begin();
	while ( hasActiveTasks() && it != _activeTasks.end() ) {
		Pointer< Task > task = *it;
		++it;
		task->update();
	}

	cleanup();

	return hasActiveTasks();
}

void RunLoop::stop( void )
{
	for ( auto task : _activeTasks ) {
		_killedTasks.push_back( task );
	}

	_activeTasks.clear();
}

void RunLoop::cleanup( void )
{
	auto it = _killedTasks.begin();
	while ( it != _killedTasks.end() ) {
		Pointer< Task > task = *it;
		++it;
		task->stop();
	}
	_killedTasks.clear();
}

