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

#ifndef CRIMILD_FOUNDATION_POLICIES_THREADING_
#define CRIMILD_FOUNDATION_POLICIES_THREADING_

#include <thread>
#include <mutex>

namespace crimild {

	namespace policies {

		class SingleThreaded {
		public:
			class Lock {
			public:
				explicit Lock( const SingleThreaded * ) { }
				~Lock( void ) { }
			};
		};

		class ObjectLevelLockable {
		public:
			using Mutex = std::mutex;

		public:
			class Lock {
			public:
				Lock( const ObjectLevelLockable *host )
				{
					_host = host;
					_host->_mutex.lock();
				}

				Lock( const Lock & ) = delete;

				~Lock( void )
				{
					_host->_mutex.unlock();
					_host = nullptr;
				}

			private:
				const ObjectLevelLockable *_host = nullptr;
			};

		private:
			mutable Mutex _mutex;
		};

		class ClassLevelLockable {
			using Mutex = std::mutex;
			
		public:
			class Lock {
			public:
				explicit Lock( const ClassLevelLockable * )
				{
					_mutex.lock();
				}

				Lock( const Lock & ) = delete;

				~Lock( void )
				{
					_mutex.unlock();
				}
			};

		private:
			static Mutex _mutex;
		};

	}

}

#endif

