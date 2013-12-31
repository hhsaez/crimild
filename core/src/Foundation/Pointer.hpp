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

#ifndef CRIMILD_CORE_FOUNDATION_POINTER_
#define CRIMILD_CORE_FOUNDATION_POINTER_

#include "SharedObject.hpp"

namespace crimild {

	/**
		\brief A smart pointer implementation

		This class is basically a smart pointer for any Object-derived class,
		handling the calls to addRef() and release() methods
		automatically and thus avoiding the need to doing it manually, which
		is very error prone.

		\remarks While the template argument does not enforce a specific
		object type, this class is intended to be used by
		instances of Object of any of its sub-classes.
	 */
	template< class T >
	class Pointer {
	public:
		/**
			\brief Default constructor
		 */
		Pointer( void )
			: _pointee( nullptr )
		{

		}

		/**
			\brief Explicit constructor

			If the pointee object is a valid one, the handler invokes
			incReferences().
		 */
		Pointer( T *pointee )
			: _pointee( pointee )
		{
			if ( _pointee ) {
				_pointee->retain();
			}
		}

		/**
			\brief Explicit constructor

			If the pointee object is a valid one, the handler invokes
			incReferences().
		 */
		explicit Pointer( const T *pointee )
			: _pointee( pointee )
		{
			if ( _pointee ) {
				_pointee->retain();
			}
		}

		/**
			\brief Explicit constructor

			The template construction allows for down-casting a pointer of
			type U to its base class of type T

			If the pointee object is a valid one, the handler invokes
			incReferences().
		 */
		template< class U >
		Pointer( U *pointee )
			: _pointee( pointee )
		{
			if ( _pointee ) {
				_pointee->retain();
			}
		}

		/**
			\brief Copy constructor
		 */
		Pointer( const Pointer &handler )
			: _pointee( handler._pointee )
		{
			if ( _pointee ) {
				_pointee->retain();
			}
		}

		/**
			\brief Copy constructor

			The template construction allows for down-casting a pointer of
			type U to its base class of type T

			If the pointee object is a valid one, the handler invokes
			incReferences().
		 */
		template< class U >
		Pointer( Pointer< U > &handler )
			: _pointee( handler.get() )
		{
			if ( _pointee ) {
				_pointee->retain();
			}
		}

		virtual ~Pointer( void )
		{
			if ( _pointee ) {
				_pointee->release();
			}
		}

		/**
			\brief Assignment

			Decreases the reference count of the existing _pointee object
			before the assignment. Then, the handler class incReferences()
			for the new _pointee object
		 */
		Pointer &operator=( T *p )
		{
			if ( p != _pointee ) {
				if ( _pointee ) {
					_pointee->release();
				}

				_pointee = p;

				if ( _pointee ) {
					_pointee->retain();
				}
			}

			return *this;
		}

		/**
			\brief Copy

			Decreases the reference count of the existing _pointee object
			before the assignment. Then, the handler class incReferences()
			for the new _pointee object
		*/
		Pointer &operator=( const Pointer &p )
		{
			if ( p._pointee != _pointee ) {
				if ( _pointee ) {
					_pointee->release();
				}

				_pointee = p._pointee;

				if ( _pointee ) {
					_pointee->retain();
				}
			}

			return *this;
		}

		/**
			\brief Copy

			This implementation has to do with inheriance.

			Decreases the reference count of the existing _pointee object
			before the assignment. Then, the handler class incReferences()
			for the new _pointee object
		*/
		template< class U >
		Pointer &operator=( const Pointer< U > &p )
		{
			T *ptr = ( U * ) p;
			if ( ptr != _pointee ) {
				if ( _pointee ) {
					_pointee->release();
				}

				_pointee = ptr;

				if ( _pointee ) {
					_pointee->retain();
				}
			}

			return *this;
		}

		T *operator->( void )
		{
			return _pointee;
		}

		const T *operator->( void ) const
		{
			return _pointee;
		}

		T &operator*( void )
		{
			return *_pointee;
		}

		const T &operator*( void ) const
		{
			return *_pointee;
		}

		/**
			\brief Not operator
		 */
		bool operator!( void ) const { return !_pointee; }

		/**
			\brief Equality
		 */
		bool operator==( T *p ) const { return _pointee == p; }

		/**
			\brief Not operator
		 */
		bool operator!=( T *p ) const { return _pointee != p; }

		bool operator==( const Pointer &p ) const { return _pointee == p._pointee; }

		bool operator!=( const Pointer &p ) const { return _pointee != p._pointee; }

		operator T*( void ) const
		{
			return _pointee;
		}

		T *get( void ) { return _pointee; }

	private:
		T *_pointee;
	};

}

#endif

