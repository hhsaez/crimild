#ifndef CRIMILD_CORE_CONCURRENCY_JOB_SCHEDULER_
#define CRIMILD_CORE_CONCURRENCY_JOB_SCHEDULER_

#include "Job.hpp"
#include "WorkStealingDeque.hpp"

#include "Foundation/Singleton.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <map>

namespace crimild {

	namespace concurrency {

		class JobScheduler : public DynamicSingleton< JobScheduler > {
		public:
			JobScheduler( void );
			virtual ~JobScheduler( void );

			/**
			   \remarks This method must be invoked from the main thread
			 */
			bool start( int numWorkers = -1 );
			void stop( void );

		public:
			enum class State {
				INITIALIZING,
				RUNNING,
				STOPPING,
				STOPPED
			};

			State getState( void ) const { return _state; }

		private:
			State _state = State::INITIALIZING;

        public:
            using WorkerId = std::thread::id;
            
            WorkerId getWorkerId( void ) const;
            
			int getNumWorkers( void ) const { return _workers.size(); }
            
            bool isMainWorker( void ) const { return getWorkerId() == _mainWorkerId; }

		private:
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

		private:
			struct WorkerStat {
				size_t jobCount = 0;
			};
			
			std::map< WorkerId, WorkerStat > _workerStats;
            
        private:
            std::mutex _mutex;
		};

	}

}

#endif

