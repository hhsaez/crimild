#ifndef CRIMILD_CORE_CONCURRENCY_JOB_
#define CRIMILD_CORE_CONCURRENCY_JOB_

#include "Foundation/NamedObject.hpp"
#include "Foundation/SharedObject.hpp"

#include <atomic>
#include <functional>

namespace crimild {

	namespace concurrency {
        
        class Job;
        
        using JobPtr = SharedPointer< Job >;

		/**
		   \brief Callback for a job
		 */
		using JobCallback = std::function< void( void ) >;

		/**
		   \brief Callback for job continuations
		 */
		using JobContinuationCallback = std::function< void( void ) >;

		/**
		   \brief Describes a job that can be executed concurrently
		 */
		class Job : public SharedObject, public NamedObject {
		public:
			Job( void );
			virtual ~Job( void );

			void reset( void );
			void reset( JobCallback const &callback );
			void reset( JobPtr const &parent, JobCallback const &callback );
			
			void execute( void );
			void finish( void );

			bool isCompleted( void ) const { return _childCount == 0; }

		private:
			JobCallback _callback;

		public:
			Job *getParent( void ) const { return _parent; }

		private:
			Job *_parent = nullptr;

			/**
			   \name Child management
			 */
		public:
			void increaseChildCount( void );
			void decreaseChildCount( void );
			size_t getChildCount( void ) const { return _childCount; }

		private:
			/**
			   \brief Keep track of child jobs

			   The starting value for the _childCount variable depends on
			   whether or not the job has anything to do. If the job was
			   created with a valid callback, the child count starts at 1.
			   Otherwise, it starts at zero indicating this job does nothing
			   (and will probably be used to manage child jobs)
			 */
            std::atomic< size_t > _childCount;

			/**
			   \name Continuation support
			*/
			//@{
			
		public:
			void attachContinuation( JobContinuationCallback const &continuation );

		private:
			std::vector< JobContinuationCallback > _continuations;

			//@}
		};

	}

}

#endif

