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

#include "Input.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"

using namespace crimild;
using namespace crimild::messaging;

Input::Input( void )
{
	reset( 256, 8 );

	auto self = this;

	registerMessageHandler< KeyPressed >( [self]( KeyPressed const &msg ) {
		int key = msg.key;
		self->_keys[ key ] = true;
	});

	registerMessageHandler< KeyReleased >( [self]( KeyReleased const &msg ) {
		int key = msg.key;
		self->_keys[ key ] = false;
	});

	registerMessageHandler< MouseButtonDown >( [self]( MouseButtonDown const &msg ) {
		int button = msg.button;
		self->_mouseButtons[ button ] = true;
	});

	registerMessageHandler< MouseButtonUp >( [self]( MouseButtonUp const &msg ) {
		int button = msg.button;
		self->_mouseButtons[ button ] = false;
	});

	registerMessageHandler< MouseMotion >( [self]( MouseMotion const &msg ) {
        Vector2f pos( msg.x, msg.y );
        self->_mouseDelta = pos - self->_mousePos;
        self->_mousePos = pos;
        
        Vector2f npos( msg.nx, msg.ny );
        self->_normalizedMouseDelta = npos - self->_normalizedMousePos;
        self->_normalizedMousePos = npos;
	});

	setAxis( AXIS_HORIZONTAL, 0.0f );
	setAxis( AXIS_VERTICAL, 0.0f );
}

Input::~Input( void )
{

}

void Input::reset( void )
{
	reset( _keys.size(), _mouseButtons.size() );
}

void Input::reset( int keyCount, int mouseButtonCount )
{
	_keys.resize( keyCount );
	for ( int i = 0; i < keyCount; i++ ) {
		_keys[ i ] = false;
	}

	_mousePos = Vector2f( 0.0f, 0.0f );
	_mouseDelta = Vector2f( 0.0f, 0.0f );
	_normalizedMousePos = Vector2f( 0.0f, 0.0f );
	_normalizedMouseDelta = Vector2f( 0.0f, 0.0f );

	_mouseButtons.resize( mouseButtonCount );
	for ( int i = 0; i < mouseButtonCount; i++ ) {
		_mouseButtons[ i ] = false;
	}
    
    _mouseCursorMode = MouseCursorMode::NORMAL;
}

