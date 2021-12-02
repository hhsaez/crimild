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

#ifndef CRIMILD_RENDERING_CULL_FACE_STATE_
#define CRIMILD_RENDERING_CULL_FACE_STATE_

#include "RenderState.hpp"

namespace crimild {
    
	class [[ deprecated ]] CullFaceState : public RenderState {
    public:
        // TODO: these should be 'const'
        static SharedPointer< CullFaceState > DISABLED;
        static SharedPointer< CullFaceState > DISABLED_CLOCKWISE;
        static SharedPointer< CullFaceState > ENABLED_BACK;
        static SharedPointer< CullFaceState > ENABLED_FRONT;

	public:
        enum class CullFaceMode : uint8_t {
        	BACK,
        	FRONT,
        	FRONT_AND_BACK
		};

        enum class FrontFace : uint8_t {
			COUNTER_CLOCKWISE,
            CLOCKWISE,
        };
        
	public:
		CullFaceState( bool enabled = true, CullFaceMode mode = CullFaceMode::BACK, FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE );
		virtual ~CullFaceState( void );

		CullFaceMode getCullFaceMode( void ) const { return _cullFaceMode; }
		void setCullFaceMode( CullFaceMode value ) { _cullFaceMode = value; }

        FrontFace getFrontFace( void ) const { return _frontFace; }
        void setFrontFace( FrontFace value ) { _frontFace = value; }

	private:
		CullFaceMode _cullFaceMode = CullFaceMode::BACK;
        FrontFace _frontFace = FrontFace::COUNTER_CLOCKWISE;
	};

}

#endif

