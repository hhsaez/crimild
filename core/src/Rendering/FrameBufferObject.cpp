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

#include "FrameBufferObject.hpp"

using namespace crimild;

FrameBufferObject::FrameBufferObject( int width, int height,
									  int redBits, int greenBits, int blueBits, int alphaBits,
						   			  int depthBits, int stencilBits )
	: _width( width ),
	  _height( height ),
	  _redBits( redBits ),
	  _greenBits( greenBits ),
	  _blueBits( blueBits ),
	  _alphaBits( alphaBits ),
	  _depthBits( depthBits ),
	  _stencilBits( stencilBits ),
	  _clearColor( 0.0f, 0.0f, 0.0f, 1.0f ),
	  _texture( new Texture( nullptr ) )
{

}
FrameBufferObject::FrameBufferObject( FrameBufferObject *fb )
	: _width( fb->_width ),
	  _height( fb->_height ),
	  _redBits( fb->_redBits ),
	  _greenBits( fb->_greenBits ),
	  _blueBits( fb->_blueBits ),
	  _alphaBits( fb->_alphaBits ),
	  _depthBits( fb->_depthBits ),
	  _stencilBits( fb->_stencilBits ),
	  _clearColor( fb->_clearColor ),
	  _texture( new Texture( nullptr ) )
{

}

FrameBufferObject::~FrameBufferObject( void ) 
{

}

