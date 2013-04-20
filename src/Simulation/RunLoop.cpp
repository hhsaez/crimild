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

using namespace Crimild;

RunLoop::RunLoop( void )
{

}

RunLoop::~RunLoop( void )
{
	if ( hasActiveTasks() ) {
		stop();
	}
}

void RunLoop::startTask( TaskPtr task )
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
	_activeTasks.insert( it, task );
	task->start();
}

void RunLoop::stopTask( TaskPtr task )
{
	if ( isTaskKilled( task ) ) {
		return;
	}

	_killedTasks.push_back( task );

	if ( isTaskActive( task ) ) {
		_activeTasks.remove( task );
	}
	else {
		_suspendedTasks.remove( task );
	}

	task->stop();
}

void RunLoop::suspendTask( TaskPtr task )
{
	if ( isTaskActive( task ) ) {
		_suspendedTasks.push_back( task );
		_activeTasks.remove( task );
		task->suspend();
	}
}

void RunLoop::resumeTask( TaskPtr task )
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
	_activeTasks.insert( it, task );
	_suspendedTasks.remove( task );

	task->resume();
}

bool RunLoop::isTaskActive( TaskPtr task ) const
{
	return std::find( std::begin( _activeTasks ), std::end( _activeTasks ), task ) != std::end( _activeTasks );
}

void RunLoop::foreachActiveTask( std::function< void ( TaskPtr &task ) > callback )
{
	for ( auto task : _activeTasks ) {
		callback( task );
	}
}

bool RunLoop::isTaskKilled( TaskPtr task ) const
{
	return std::find( std::begin( _killedTasks ), std::end( _killedTasks ), task ) != std::end( _killedTasks );
}

void RunLoop::foreachKilledTask( std::function< void ( TaskPtr &task ) > callback )
{
	for ( auto task : _killedTasks ) {
		callback( task );
	}
}

bool RunLoop::isTaskSuspended( TaskPtr task ) const
{
	return std::find( std::begin( _suspendedTasks ), std::end( _suspendedTasks ), task ) != std::end( _suspendedTasks );
}

void RunLoop::foreachSuspendedTask( std::function< void ( TaskPtr &task ) > callback )
{
	for ( auto task : _suspendedTasks ) {
		callback( task );
	}
}

bool RunLoop::update( void )
{
	auto it = _activeTasks.begin();
	while ( it != _activeTasks.end() ) {
		Task *task = ( *it ).get();
		++it;
		task->update();
	}

	return hasActiveTasks();
}

void RunLoop::stop( void )
{
	for ( auto task : _activeTasks ) {
		_killedTasks.push_back( task );
		task->stop();
	}

	_activeTasks.clear();
}

