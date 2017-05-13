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

#ifndef CRIMILD_CORE_CONCURRENCY_JOB_SCHEDULER_
#define CRIMILD_CORE_CONCURRENCY_JOB_SCHEDULER_

#include "Job.hpp"
#include "WorkStealingDeque.hpp"

#include "Foundation/Singleton.hpp"
#include "Foundation/ConcurrentList.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <map>

namespace crimild {

	namespace concurrency {

        /**
            \brief A scheduler for asynchronous jobs
         */
		class JobScheduler : public DynamicSingleton< JobScheduler > {
		public:
            /**
                \brief Default constructor
             
                \remarks The constructor does not automatically start workers. You
                need to call the start() method for doing that.
             */
			JobScheduler( void );
            
            /**
                \brief Destructor
             
                \remarks The destructor does not stop workers.  Invoke
                the stop() method manually in order join worker threads.
             */
			virtual ~JobScheduler( void );
            
            void configure( int numWorkers = -1 );

			/**
                \brief Start all worker threads
             
                \remarks This method must be invoked from the main thread.
			 */
			bool start( void );
            
            /**
                \brief Stop all worker threads and wait for them to complete
             
                \remarks Invoke this method from the main thread
             */
			void stop( void );

		public:
			enum class State {
				INITIALIZING,
				RUNNING,
				STOPPING,
				STOPPED
			};
            
            bool isRunning( void ) const { return _state == State::RUNNING; }

			State getState( void ) const { return _state; }

		private:
			State _state = State::INITIALIZING;

        public:
            using WorkerId = std::thread::id;
            
            WorkerId getWorkerId( void ) const;
            
			int getNumWorkers( void ) const { return _numWorkers; }
            
            bool isMainWorker( void ) const { return getWorkerId() == _mainWorkerId; }

		private:
            int _numWorkers;
			std::vector< std::thread > _workers;
            WorkerId _mainWorkerId;

		private:
			using WorkerJobQueue = WorkStealingQueue< JobPtr >;

			void initWorker( bool mainWorker = false );
			WorkerJobQueue *getWorkerJobQueue( void );
			WorkerJobQueue *getRandomJobQueue( void );

			void worker( void );

		private:
			std::map< WorkerId, SharedPointer< WorkerJobQueue >> _workerJobQueues;

		public:
			void schedule( JobPtr const &job );
			void wait( JobPtr const &job );

			void yield( void );

		private:
			JobPtr getJob( void );
			
			bool executeNextJob( void );
			void execute( JobPtr const &job );

		public:
			struct WorkerStat {
				size_t jobCount = 0;
			};
			
			void eachWorkerStat( std::function< void( WorkerId, const WorkerStat & ) > const &callback ) const;
			
			void clearWorkerStats( void );

		private:
			std::map< WorkerId, WorkerStat > _workerStats;
            
        private:
            std::mutex _mutex;
            
        public:
            void delaySync( JobPtr const &job );
            void delayAsync( JobPtr const &job );
            
            void executeDelayedJobs( void );

        private:
            ConcurrentList< JobPtr > _delayedSyncJobs;
            ConcurrentList< JobPtr > _delayedAsyncJobs;
		};

	}

}

#endif

