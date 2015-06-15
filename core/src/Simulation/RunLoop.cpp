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

#include "Foundation/Profiler.hpp"

#include <iostream>
#include <algorithm>

using namespace crimild;

RunLoop::RunLoop( std::string name )
	: NamedObject( name )
{

}

RunLoop::~RunLoop( void )
{
	if ( hasActiveTasks() ) {
		stop();
	}

	cleanup();
}

void RunLoop::startTask( TaskPtr const &task )
{
	/*
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

	task->setRunLoop( getShared< RunLoop>() );
	task->start();
	*/
}

void RunLoop::stopTask( TaskPtr const &task )
{
	/*
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
	*/
}

void RunLoop::suspendTask( TaskPtr const &task )
{
	/*
	if ( isTaskActive( task ) ) {
		_suspendedTasks.push_back( task );
		_activeTasks.remove( task );
		task->suspend();
	}
	*/
}

void RunLoop::resumeTask( TaskPtr const &task )
{
	/*
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
	*/
}

bool RunLoop::isTaskActive( TaskPtr const &task ) const
{
	return std::find( std::begin( _activeTasks ), std::end( _activeTasks ), task ) != std::end( _activeTasks );
}

void RunLoop::foreachActiveTask( std::function< void ( TaskPtr const &task ) > callback )
{
    auto ts = _activeTasks;
	for ( auto task : ts ) {
		callback( task );
	}
}

bool RunLoop::isTaskKilled( TaskPtr const &task ) const
{
	return std::find( std::begin( _killedTasks ), std::end( _killedTasks ), task ) != std::end( _killedTasks );
}

void RunLoop::foreachKilledTask( std::function< void ( TaskPtr const &task ) > callback )
{
    auto ks = _killedTasks;
	for ( auto task : ks ) {
		callback( task );
	}
}

bool RunLoop::isTaskSuspended( TaskPtr const &task ) const
{
	return std::find( std::begin( _suspendedTasks ), std::end( _suspendedTasks ), task ) != std::end( _suspendedTasks );
}

void RunLoop::foreachSuspendedTask( std::function< void ( TaskPtr const &task ) > callback )
{
    auto ts = _suspendedTasks;
	for ( auto task : ts ) {
		callback( task );
	}
}

bool RunLoop::update( void )
{
	CRIMILD_PROFILE( getName() + " Update" );

	auto ts = _activeTasks;
	for ( auto task : ts ) {
		task->update();
	}

	cleanup();

	return hasActiveTasks();
}

void RunLoop::stop( void )
{
    auto ts = _activeTasks;
	for ( auto task : ts ) {
		_killedTasks.push_back( task );
	}

	_activeTasks.clear();
}

void RunLoop::cleanup( void )
{
	auto it = _killedTasks.begin();
	while ( it != _killedTasks.end() ) {
		auto task = *it;
		++it;
		task->stop();
		task->setRunLoop( RunLoopPtr() );
	}
	_killedTasks.clear();
}

ThreadedRunLoop::ThreadedRunLoop( std::string name, bool startImmediately )
	: RunLoop( name )
{
    if ( startImmediately ) run();
}

ThreadedRunLoop::~ThreadedRunLoop( void )
{
    
}

void ThreadedRunLoop::run( void )
{
    _done = false;
    _thread = std::move( std::thread( [&]() {
        while ( !_done ) {
            update();
        }
    }));
}

void ThreadedRunLoop::stop( void )
{
    _done = true;
    RunLoop::stop();
    
    if ( _thread.joinable() ) {
	    _thread.join();
    }
}

