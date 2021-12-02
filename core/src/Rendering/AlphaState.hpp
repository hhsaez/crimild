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

#ifndef CRIMILD_RENDERING_ALPHA_STATE_
#define CRIMILD_RENDERING_ALPHA_STATE_

#include "RenderState.hpp"

namespace crimild {

	class AlphaState : public RenderState {
    public:
        // TODO: these should be const
        static SharedPointer< AlphaState > DISABLED;
        static SharedPointer< AlphaState > ENABLED;
        static SharedPointer< AlphaState > ENABLED_SRC_COLOR_ONLY;
        static SharedPointer< AlphaState > ENABLED_ADDITIVE_BLEND;
		static SharedPointer< AlphaState > ENABLED_MULTIPLY_BLEND;
        
	public:
		enum class SrcBlendFunc : uint8_t {
			ZERO,
			ONE,
			SRC_COLOR,
			ONE_MINUS_SRC_COLOR,
			DST_COLOR,
			ONE_MINUS_DST_COLOR,
			SRC_ALPHA,
			ONE_MINUS_SRC_ALPHA,
			DST_ALPHA,
			ONE_MINUS_DST_ALPHA,
			SRC_ALPHA_SATURATE
		};

		enum class DstBlendFunc : uint8_t {
			ZERO,
			ONE,
			SRC_COLOR,
			ONE_MINUS_SRC_COLOR,
			SRC_ALPHA,
			ONE_MINUS_SRC_ALPHA,
			DST_ALPHA,
			ONE_MINUS_DST_ALPHA
		};

	public:
		AlphaState( bool enabled = false, 
					SrcBlendFunc srcBlendFunc = SrcBlendFunc::SRC_ALPHA, 
					DstBlendFunc dstBlendFunct = DstBlendFunc::ONE_MINUS_SRC_ALPHA );

		virtual ~AlphaState( void );

		SrcBlendFunc getSrcBlendFunc( void ) const { return _srcBlendFunc; }
		void setSrcBlendFunc( SrcBlendFunc value ) { _srcBlendFunc = value; }

		DstBlendFunc getDstBlendFunc( void ) const { return _dstBlendFunc; }
		void setDstBlendFunc( DstBlendFunc value ) { _dstBlendFunc = value; }

	private:
		SrcBlendFunc _srcBlendFunc = SrcBlendFunc::SRC_ALPHA;
		DstBlendFunc _dstBlendFunc = DstBlendFunc::ONE_MINUS_SRC_ALPHA;
	};

}

#endif

