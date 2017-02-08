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

#ifndef CRIMILD_FOUNDATION_CONCURRENT_QUEUE_
#define CRIMILD_FOUNDATION_CONCURRENT_QUEUE_

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace crimild {

	template< typename T >
	class ConcurrentQueue {
	private:
		using Queue = std::queue< T >;
		using Mutex = std::mutex;
		using ScopedLock = std::unique_lock< Mutex >;
		using Condition = std::condition_variable;

	public:
		bool empty( void ) const 
		{
			ScopedLock lock( _mutex );
			return _queue.empty();
		}

		unsigned int size( void ) const
		{
			ScopedLock lock( _mutex );
			return _queue.size();
		}

		void push( T const &value ) 
		{
			ScopedLock lock( _mutex );
			_queue.push( value );
			lock.unlock();

			_condition.notify_one();
		}

		bool tryPop( T &result )
		{
			ScopedLock lock( _mutex );

			if ( _queue.empty() ) {
				return false;
			}

			result = _queue.front();
			_queue.pop();
			return true;
		}

		T waitPop( void )
		{
			ScopedLock lock( _mutex );

			while ( _queue.empty() ) {
				_condition.wait( lock );
			}

			T result( _queue.front() );
			_queue.pop();

			return result;
		}

	private:
		Queue _queue;
		mutable Mutex _mutex;
		Condition _condition;
	};

}

#endif

