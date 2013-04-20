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

#ifndef CRIMILD_SIMULATION_RUN_LOOP_
#define CRIMILD_SIMULATION_RUN_LOOP_

#include "Task.hpp"

#include <functional>
#include <list>

namespace Crimild {

	class RunLoop {
	public:
		RunLoop( void );
		virtual ~RunLoop( void );

		void startTask( TaskPtr task );
		void stopTask( TaskPtr task );
		void suspendTask( TaskPtr task );
		void resumeTask( TaskPtr task );

		bool update( void );
		void stop( void );

		bool hasActiveTasks( void ) const { return _activeTasks.size() > 0; }
		bool isTaskActive( TaskPtr task ) const;
		void foreachActiveTask( std::function< void ( TaskPtr &task ) > callback );

		bool hasKilledTasks( void ) const { return _killedTasks.size() > 0; }
		bool isTaskKilled( TaskPtr task ) const;
		void foreachKilledTask( std::function< void ( TaskPtr &task ) > callback );

		bool hasSuspendedTasks( void ) const { return _suspendedTasks.size() > 0; }
		bool isTaskSuspended( TaskPtr task ) const;
		void foreachSuspendedTask( std::function< void ( TaskPtr &task ) > callback );

	private:
		std::list< TaskPtr > _activeTasks;
		std::list< TaskPtr > _killedTasks;
		std::list< TaskPtr > _suspendedTasks;
	};

	typedef std::shared_ptr< RunLoop > RunLoopPtr;

}

#endif

