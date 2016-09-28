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

#ifndef CRIMILD_RENDERING_COLOR_MASK_STATE_
#define CRIMILD_RENDERING_COLOR_MASK_STATE_

#include "RenderState.hpp"

namespace crimild {
    
	class ColorMaskState : public RenderState {
    public:
        static const SharedPointer< ColorMaskState > DISABLED;
        static const SharedPointer< ColorMaskState > ENABLED;

	public:
		ColorMaskState( bool enabled, bool rMask, bool gMask, bool bMask, bool aMask );
		virtual ~ColorMaskState( void );

		void setRMask( bool value ) { _rMask = value; }
		bool getRMask( void ) const { return _rMask; }

		void setGMask( bool value ) { _gMask = value; }
		bool getGMask( void ) const { return _gMask; }

		void setBMask( bool value ) { _bMask = value; }
		bool getBMask( void ) const { return _bMask; }

		void setAMask( bool value ) { _aMask = value; }
		bool getAMask( void ) const { return _aMask; }

	private:
		bool _rMask;
		bool _gMask;
		bool _bMask;
		bool _aMask;
	};

}

#endif

