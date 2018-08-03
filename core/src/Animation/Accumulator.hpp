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

#ifndef CRIMILD_ANIMATION_ACCUMULATOR_
#define CRIMILD_ANIMATION_ACCUMULATOR_

#include "Foundation/SharedObject.hpp"
#include "Mathematics/Vector.hpp"
#include "Mathematics/Quaternion.hpp"
#include "Mathematics/Interpolation.hpp"

namespace crimild {

	namespace animation {

		class Accumulator : public SharedObject {
		protected:
			Accumulator( void ) { }

		public:
			virtual ~Accumulator( void ) { }

			virtual void lerp( Accumulator *other, crimild::Real32 factor ) = 0;
			virtual void add( Accumulator *other, crimild::Real32 strength ) = 0;
		};

		template< typename T >
		class AccumulatorImpl : public Accumulator {
		public:
			AccumulatorImpl( void ) { }

			virtual ~AccumulatorImpl( void ) { }

			void set( const T &v ) { _value = v; }

			T get( void ) const { return _value; }

		private:
			T _value;

		public:
			virtual void lerp( Accumulator *other, crimild::Real32 factor ) override
			{
				auto otherAcc = static_cast< AccumulatorImpl< T > * >( other );

				Interpolation::linear( _value, otherAcc->_value, factor, _value );
			}

			virtual void add( Accumulator *other, crimild::Real32 strength ) override
			{
				auto otherAcc = static_cast< AccumulatorImpl< T > * >( other );
				
				_value = _value + otherAcc->_value;				
			}
		};

		template<>
		class AccumulatorImpl< Quaternion4f > : public Accumulator {
		public:
			AccumulatorImpl( void ) { }

			virtual ~AccumulatorImpl( void ) { }

			void set( const Quaternion4f &v ) { _value = v; }

			Quaternion4f get( void ) const { return _value; }

		private:
			Quaternion4f _value;
			
		public:
			virtual void lerp( Accumulator *other, crimild::Real32 factor ) override
			{
				auto otherAcc = static_cast< AccumulatorImpl< Quaternion4f > * >( other );

				Interpolation::slerp( _value, otherAcc->_value, factor, _value );
			}
			
			virtual void add( Accumulator *other, crimild::Real32 strength ) override
			{
				auto otherAcc = static_cast< AccumulatorImpl< Quaternion4f > * >( other );
				
				_value = _value * otherAcc->_value;				
			}
		};

	}

}

#endif

