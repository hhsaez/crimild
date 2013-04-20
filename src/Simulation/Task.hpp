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

#ifndef CRIMILD_SIMULATION_TASK_
#define CRIMILD_SIMULATION_TASK_

#include <memory>

namespace Crimild {

	class Task {
	protected:
		Task( int priority = 0 );

	public:
		virtual ~Task( void );

		int getPriority( void ) const { return _priority; }

	private:
		int _priority;

	public:
		virtual void start( void ) { }
		virtual void stop( void ) { }
		virtual void suspend( void ) { }
		virtual void resume( void ) { }
		virtual void update( void ) { }
	};

	typedef std::shared_ptr< Task > TaskPtr;

}

#endif

