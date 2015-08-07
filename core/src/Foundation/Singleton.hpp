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

#ifndef CRIMILD_CORE_FOUNDATION_SINGLETON_
#define CRIMILD_CORE_FOUNDATION_SINGLETON_

#include <cassert>

namespace crimild {

	// thread safe
	template< class ObjectType >
	class SingletonStackStoragePolicy {
	private:
		// forces singleton to be initialized before main
		static ObjectType _dummyInstance;

	public:
		static ObjectType *getInstance( void )
		{
  			static ObjectType instance;	// actual instance
  			return &instance;
		}
	};

	template< class ObjectType >
	ObjectType SingletonStackStoragePolicy< ObjectType >::_dummyInstance;

	// non-thread safe
	template< class ObjectType >
	class SingletonHeapStoragePolicy {
	private:
		static ObjectType *_instance;

	public:
		static ObjectType *getInstance( void )
		{
			return _instance;
		}

	protected:
		SingletonHeapStoragePolicy( void )
		{
//			assert( _instance == nullptr && "Singleton instance already set" );
			_instance = static_cast< ObjectType * >( this );
		}

	public:
		virtual ~SingletonHeapStoragePolicy( void )
		{
			_instance = nullptr;
		}
	};

	template< class ObjectType >
	ObjectType *SingletonHeapStoragePolicy< ObjectType >::_instance = nullptr;

	template< 
		class ObjectType,
		template< class > class SingletonStoragePolicy
	>
	class Singleton : public SingletonStoragePolicy< ObjectType > {
	public:
		Singleton( void )
		{

		}

		virtual ~Singleton( void )
		{

		}
	};

	template< class ObjectType >
	using StaticSingleton = Singleton< ObjectType, SingletonStackStoragePolicy >;

	template< class ObjectType >
	using DynamicSingleton = Singleton< ObjectType, SingletonHeapStoragePolicy >;

}

#endif

