#ifndef CRIMILD_CORE_CONCURRENCY_
#define CRIMILD_CORE_CONCURRENCY_

#include "Job.hpp"

namespace crimild {

	namespace concurrency {

		SharedPointer< Job > async( SharedPointer< Job > const &job );

		SharedPointer< Job > async( JobCallback const &callback );

		SharedPointer< Job > async( SharedPointer< Job > const &parent, JobCallback const &callback );

		SharedPointer< Job > sync_frame( void );

		SharedPointer< Job > async_frame( void );

		void wait( SharedPointer< Job > const &job );

	}

}

#endif

