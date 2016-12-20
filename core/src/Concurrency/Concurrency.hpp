#ifndef CRIMILD_CORE_CONCURRENCY_
#define CRIMILD_CORE_CONCURRENCY_

#include "Job.hpp"

namespace crimild {

	namespace concurrency {

		JobPtr async( void );

		JobPtr async( JobCallback const &callback );

		JobPtr async( JobPtr const &parent, JobCallback const &callback );

		JobPtr sync_frame( void );

		JobPtr async_frame( void );

		void wait( JobPtr const &job );

	}

}

#endif

