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

#ifndef CRIMILD_CONCURRENCY_ASYNC_
#define CRIMILD_CONCURRENCY_ASYNC_

#include <functional>

namespace crimild {
    
    class AsyncDispatchPolicy {
    public:
        enum {
            NONE = 0x0,
            
            THREAD_SAFE = 0x1 << 0,
            SYNC_FRAME = 0x1 << 1,
            
            MAIN_QUEUE = NONE,
            MAIN_QUEUE_SYNC = SYNC_FRAME,
            
            BACKGROUND_QUEUE = THREAD_SAFE,
            BACKGROUND_QUEUE_SYNC = BACKGROUND_QUEUE | SYNC_FRAME,
            
            ALL = ~NONE
        };
    };
    
    void async( unsigned int dispatchPolicy, std::function< void( void ) > onRun, std::function< void( void ) > onCompleted = nullptr );
    
    void async( std::function< void( void ) > onRun, std::function< void( void ) > onCompleted = nullptr );
    
}

#endif

