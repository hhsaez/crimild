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

#ifndef CRIMILD_ANIMATION_CLIP_
#define CRIMILD_ANIMATION_CLIP_

#include "Coding/Codable.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/Containers/Array.hpp"

namespace crimild {

	namespace animation {

		class Animation;
		class Channel;

		class Clip :
			public coding::Codable,
			public NamedObject {
			CRIMILD_IMPLEMENT_RTTI( crimild::animation::Clip )
			
		public:
			explicit Clip( std::string name );
			explicit Clip( std::string name, SharedPointer< Channel > const &channel );
			virtual ~Clip( void );

			void setDuration( crimild::Real32 duration ) { _duration = duration; }
			crimild::Real32 getDuration( void ) const { return _duration; }

			void setFrameRate( crimild::Real32 frameRate ) { _frameRate = frameRate; }
			crimild::Real32 getFrameRate( void ) const { return _frameRate; }

		private:
			crimild::Real32 _duration = 0.0f;
			crimild::Real32 _frameRate = 1.0f;

		public:
			void addChannel( SharedPointer< Channel > const &channel );

		private:
			containers::Array< SharedPointer< Channel >> _channels;

		public:
			void evaluate( crimild::Real32 t, Animation *animation );
		};

	}

}

#endif

