/**
 * Crimild Engine is an open source scene graph based engine which purpose
 * is to fulfill the high-performance requirements of typical multi-platform
 * two and tridimensional multimedia projects, like games, simulations and
 * virtual reality.
 *
 * Copyright (C) 2006-2013 Hernan Saez - hhsaez@gmail.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
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

