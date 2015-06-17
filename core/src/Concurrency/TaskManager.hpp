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

#ifndef CRIMILD_CONCURRENCY_TASK_MANAGER_
#define CRIMILD_CONCURRENCY_TASK_MANAGER_

#include "Task.hpp"

#include "Foundation/ConcurrentQueue.hpp"
#include "Foundation/Singleton.hpp"

#include <vector>

namespace crimild {

	class TaskManager : public DynamicSingleton< TaskManager > {
	private:
		using TaskList = ConcurrentQueue< Task >;
		using ThreadGroup = std::vector< std::thread >;

		using Mutex = std::mutex;
		using Condition = std::condition_variable;
		using ScopedLock = std::unique_lock< Mutex >;

	public:
		explicit TaskManager( unsigned int numThreads = 0 );
		virtual ~TaskManager( void );

		void addTask( Task const &task );

		void start( void );
        bool isRunning( void ) const { return _running; }
        
		void pollMainTasks( void );
		
        void stop( void );

	private:
        void executeTask( Task &task );
            
		void worker( void );

		void synchronize( void );
        
        void dummyTask( void );

	private:
		bool _running = false;
		
		TaskList _tasks[ 2 ];
		unsigned int _writeList = 0;
		unsigned int _readList = 1;

		TaskList _syncTasks;

		TaskList _backgroundTasks;
		
		unsigned int _numThreads = 0;
		ThreadGroup _threads;

		mutable Mutex _syncMutex;
		Condition _syncCondition;
		unsigned int _numTasksToWaitFor = 0;
	};

}

#endif

