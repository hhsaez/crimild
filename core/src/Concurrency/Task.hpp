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

#ifndef CRIMILD_CONCURRENCY_TASK_
#define CRIMILD_CONCURRENCY_TASK_

#include <functional>

namespace crimild {
    
    class Task {
    private:
        using RunCallback = std::function< void( void ) >;
        using CompletionCallback = std::function< void( void ) >;
        
    public:
        Task( void );
        Task( Task const &other );
        ~Task( void );
        
        Task &operator=( Task const &other );
        
        bool isThreadSafe( void ) const { return _threadSafe; }
        void setThreadSafe( bool value ) { _threadSafe = value; }
        
        bool getSyncFrame( void ) const { return _syncFrame; }
        void setSyncFrame( bool value ) { _syncFrame = value; }
        
        void setRunCallback( RunCallback callback ) { _runCallback = callback; }
        void setCompletionCallback( CompletionCallback callback ) { _completionCallback = callback; }
        
        void execute( void );
        
    private:
        bool _threadSafe = false;
        bool _syncFrame = false;
        
        RunCallback _runCallback;
        CompletionCallback _completionCallback;
    };
    
#if 0

	class RunLoop;

	class Task;

	using TaskPtr = SharedPointer< Task >;

	class Task : public SharedObject {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( Task )
        
    private:
        using Mutex = std::mutex;
        using Condition = std::condition_variable;
        using ScopedLock = std::unique_lock< Mutex >;

	public:
		enum class RepeatMode {
			ONCE,
			REPEAT
		};

		enum class ThreadMode {
			FOREGROUND,
			BACKGROUND
		};

		enum class SyncMode {
			FRAME,
			NONE
		};

	protected:
		Task( void );
		explicit Task( RepeatMode repeatMode, ThreadMode threadMode, SyncMode syncMode );

	public:
		virtual ~Task( void );

		void setRepeatMode( RepeatMode value ) { _repeatMode = value; }
		RepeatMode getRepeatMode( void ) const { return _repeatMode; }

		void setThreadMode( ThreadMode value ) { _threadMode = value; }
		ThreadMode getThreadMode( void ) const { return _threadMode; }

		void setSyncMode( SyncMode value ) { _syncMode = value; }
		SyncMode getSyncMode( void ) const { return _syncMode; }

		// TODO: this should be abstract
		virtual void run( void ) { }
        
        void execute( void );
        
        void waitResult( void );
        
        // internal use only
        void notifyResult( void );

	private:
		RepeatMode _repeatMode;
		ThreadMode _threadMode;
		SyncMode _syncMode;
        
        Mutex _mutex;
        Condition _conditionVariable;

		/****************************************
		 Deprecated from here
		 ****************************************/
	protected:
		explicit Task( int priority );

	public:
		int getPriority( void ) const { return _priority; }

	private:
		int _priority;

	public:
		virtual void start( void ) { }
		virtual void stop( void ) { }
		virtual void suspend( void ) { }
		virtual void resume( void ) { }
		virtual void update( void ) { }

	public:
        void setRunLoop( SharedPointer< RunLoop > const &runLoop ) { _runLoop = runLoop; }
        SharedPointer< RunLoop > getRunLoop( void ) { return _runLoop.lock(); }

	private:
        std::weak_ptr< RunLoop > _runLoop;
	};
    
    using TaskPtr = SharedPointer< Task >;
    
#endif

}

#endif

