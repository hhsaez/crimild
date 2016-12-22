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

#ifndef CRIMILD_CORE_ASYNC_
#define CRIMILD_CORE_ASYNC_

#include "Job.hpp"

namespace crimild {

	namespace concurrency {

        /**
            \brief Creates an empty job
         
            Since the job is empty, it is not dispatched to the scheduler. This
            function is useful to create parent jobs for parallel operations
         
            \remarks The new job will not be executed since it's empty
         */
		JobPtr async( void );

        /**
            \brief Creates and dispatches an async job
         
            \remarks The job will be executed in a background thread
         */
		JobPtr async( JobCallback const &callback );

        /**
            \brief Creates and dispatches an async job linked to a parent
         
            \remarks The job will be executed in a background thread
         */
		JobPtr async( JobPtr const &parent, JobCallback const &callback );

        /**
            \brief Creates and dispatches a job in the main thread
         
            This method will create a new job and enqueue it to be executed
            at the beginning of the next simulation step. 
         
            \remarks The job will be executed in the main thread.
         */
		JobPtr sync_frame( JobCallback const &callback );

        /**
            \brief Creates and dispatches an async job in the main thread
         
            This method will create a new job and enqueue it to be executed
            at the beginning of the next simulation step. 
         
            \remarks The job will be executed in a background thread
         */
		JobPtr async_frame( JobCallback const &callback );

        /**
            \brief Waits for a job to be completed
         
            Blocks the operation of the current thread until the job
            is completed. In the meantime, other jobs may be executed
            in the current thread, if any.
         */
		void wait( JobPtr const &job );

	}

}

#endif

