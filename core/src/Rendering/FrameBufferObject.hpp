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

#ifndef CRIMILD_RENDERING_FRAME_BUFFER_OBJECT_
#define CRIMILD_RENDERING_FRAME_BUFFER_OBJECT_

#include "Catalog.hpp"
#include "Texture.hpp"

#include "Mathematics/Vector.hpp"

#include <memory>

namespace crimild {

	class FrameBufferObject : public Catalog< FrameBufferObject >::Resource {
	public:
		FrameBufferObject( int width, int height,
						   int redBits = 8, int greenBits = 8, int blueBits = 8, int alphaBits = 8,
						   int depthBits = 16, int stencilBits = 0 );
		FrameBufferObject( FrameBufferObject *fb );
		virtual ~FrameBufferObject( void );

		int getWidth( void ) const { return _width; }
		int getHeight( void ) const { return _height; }
		int getRedBits( void ) const { return _redBits; }
		int getGreenBits( void ) const { return _greenBits; }
		int getBlueBits( void ) const { return _blueBits; }
		int getAlphaBits( void ) const { return _alphaBits; }
		int getDepthBits( void ) const { return _depthBits; }
		int getStencilBits( void ) const { return _stencilBits; }

		Texture *getTexture( void ) { return _texture; }

		void setClearColor( const RGBAColorf &color ) { _clearColor = color; }
		const RGBAColorf &getClearColor( void ) const { return _clearColor; }

	private:
		int _width;
		int _height;
		int _redBits;
		int _greenBits;
		int _blueBits;
		int _alphaBits;
		int _depthBits;
		int _stencilBits;
		RGBAColorf _clearColor;
		Pointer< Texture > _texture;
	};

	typedef Catalog< FrameBufferObject > FrameBufferObjectCatalog;
}

#endif

