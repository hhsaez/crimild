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

#ifndef CRIMILD_SIMULATION_SYSTEM_
#define CRIMILD_SIMULATION_SYSTEM_

#include "Foundation/SharedObject.hpp"
#include "Foundation/RTTI.hpp"
#include "Foundation/Memory.hpp"
#include "Foundation/Types.hpp"

#include "Messaging/MessageQueue.hpp"

#include <string>

namespace crimild {
    
	class System;

	namespace messages {

		struct SystemWillStart {
			System *system;
		};

		struct SystemWillStop {
			System *system;
		};

	}

	class System : 
		public SharedObject,
		public RTTI,
		public Messenger {

	public:
		using Priority = crimild::Int32;
		
		class PriorityType {
		public:
			enum {
				HIGHEST = 0,
				HIGH = 1000,
				MEDIUM = 2000,
				LOW = 30000,
				LOWEST = 40000,
				NO_UPDATE = 50000, // system doesn't need to update

				FRAME_BEGIN = HIGHEST,

				STREAMING = HIGHEST + 10,
				INPUT = HIGH + 20,
				UI_EVENTS = HIGH + 30,

				PRE_UPDATE = MEDIUM - 10,
				UPDATE = MEDIUM,
				POST_UPDATE = LOW - 20,
				
				PRE_RENDER = LOW - 10,
				RENDER = LOW,
				POST_RENDER = LOWEST - 10,

				FRAME_END = LOWEST,
			};
		};
		
	public:
		virtual ~System( void ) = default;

		virtual bool start( void );
		virtual void update( void );
		virtual void stop( void );

		virtual Priority getInitPriority( void ) const noexcept { return PriorityType::MEDIUM; }
		virtual Priority getPriority( void ) const { return PriorityType::UPDATE; }
	};
    
    using SystemPtr = SharedPointer< System >;
    
}

#endif

