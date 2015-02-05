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

#ifndef CRIMILD_SIMULATION_RUN_LOOP_
#define CRIMILD_SIMULATION_RUN_LOOP_

#include "Task.hpp"

#include "Foundation/SharedObject.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/Pointer.hpp"
#include "Mathematics/Time.hpp"

#include <functional>
#include <list>

namespace crimild {

	class RunLoop : public SharedObject, public NamedObject {
	public:
		explicit RunLoop( std::string name );
		virtual ~RunLoop( void );

		void startTask( TaskPtr const &task );
		void stopTask( TaskPtr const &task );
		void suspendTask( TaskPtr const &task );
		void resumeTask( TaskPtr const &task );

		virtual bool update( void );
		virtual void stop( void );

		bool hasActiveTasks( void ) const { return _activeTasks.size() > 0; }
		bool isTaskActive( TaskPtr const &task ) const;
		void foreachActiveTask( std::function< void ( TaskPtr const &task ) > callback );

		bool hasKilledTasks( void ) const { return _killedTasks.size() > 0; }
		bool isTaskKilled( TaskPtr const &task ) const;
		void foreachKilledTask( std::function< void ( TaskPtr const &task ) > callback );

		bool hasSuspendedTasks( void ) const { return _suspendedTasks.size() > 0; }
		bool isTaskSuspended( TaskPtr const &task ) const;
		void foreachSuspendedTask( std::function< void ( TaskPtr const &task ) > callback );

	protected:
		void cleanup( void );

	private:
		std::list< TaskPtr > _activeTasks;
		std::list< TaskPtr > _killedTasks;
		std::list< TaskPtr > _suspendedTasks;
	};
    
    using RunLoopPtr = SharedPointer< RunLoop >;
    
    class ThreadedRunLoop : public RunLoop {
    public:
        explicit ThreadedRunLoop( std::string name, bool startImmediately = false );
        virtual ~ThreadedRunLoop( void );
        
        virtual void run( void );
        virtual void stop( void ) override;
        
    private:
        bool _done;
        std::thread _thread;
    };

}

#endif

