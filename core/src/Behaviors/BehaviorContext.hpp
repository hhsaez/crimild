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

#ifndef CRIMILD_CORE_BEHAVIORS_BEHAVIOR_CONTEXT_
#define CRIMILD_CORE_BEHAVIORS_BEHAVIOR_CONTEXT_

#include "Foundation/Types.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/RTTI.hpp"

#include "Mathematics/Clock.hpp"
#include "Mathematics/Vector.hpp"

#include <functional>
#include <vector>
#include <string>

namespace crimild {

	class Node;

	namespace behaviors {

		/**
		   \brief Execution context
		   
		   \todo Add support for value observers
		*/
		class BehaviorContext {
		public:
			BehaviorContext( void );
			virtual ~BehaviorContext( void );

			void reset( void );
			void update( const crimild::Clock &c );

		public:
			void setAgent( crimild::Node *agent ) { _agent = agent; }
			crimild::Node *getAgent( void ) { return _agent; }

		private:
			crimild::Node *_agent = nullptr;

		public:
			crimild::Bool hasTargets( void ) const { return getTargetCount() > 0; }			
			crimild::Size getTargetCount( void ) const;

			void addTarget( crimild::Node *target );
			crimild::Node *getTargetAt( crimild::Size index );
			void removeAllTargets( void );

			using TargetCallback = std::function< void( crimild::Node * ) >;
			void foreachTarget( TargetCallback const &callback );

		private:
			std::vector< crimild::Node * > _targets;
			crimild::Size _targetCount = 0;

		public:
			const crimild::Clock &getClock( void ) const { return _clock; }

		private:
			crimild::Clock _clock;

		public:
			bool hasValue( std::string key )
			{
				return _values[ key ] != "";
			}
			
			template< typename T >
			void setValue( std::string key, T value )
			{
				std::stringstream ss;
				ss << value;
				_values[ key ] = ss.str();
			}
			
			template< typename T >
			T getValue( std::string key )
			{
				if ( !hasValue( key ) ) {
					crimild::Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No context value set for key ", key );
                    return T();
				}

                T value;
				std::stringstream ss;
				ss << _values[ key ];
				ss >> value;
				return value;
			}
			
		private:
			std::map< std::string, std::string > _values;

		public:
			void dump( void ) const;
		};

		template<>
		inline void BehaviorContext::setValue< Vector3f >( std::string key, Vector3f value )
		{
			setValue< crimild::Real32 >( key + ".x", value.x() );
			setValue< crimild::Real32 >( key + ".y", value.y() );
			setValue< crimild::Real32 >( key + ".z", value.z() );
		}

		template<>
		inline crimild::Vector3f BehaviorContext::getValue( std::string key )
		{
			return Vector3f(
				getValue< crimild::Real32 >( key + ".x" ),
				getValue< crimild::Real32 >( key + ".y" ),
				getValue< crimild::Real32 >( key + ".z" )
			);
		}
		
		template<>
		inline crimild::Vector4f BehaviorContext::getValue( std::string key )
		{
			std::stringstream ss;
			ss << _values[ key ];

			float x, y, z, w;
			ss >> x >> y >> z >> w;
			return crimild::Vector4f( x, y, z, w );
		}

	}
	
}

#endif

