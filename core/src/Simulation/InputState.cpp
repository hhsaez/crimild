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

#include "InputState.hpp"

using namespace crimild;

InputState InputState::_instance;

InputState &InputState::getCurrentState( void )
{
	return _instance;
}

InputState::InputState( void )
{
	reset( 256, 8 );
}

InputState::~InputState( void )
{

}

void InputState::reset( int keyCount, int mouseButtonCount )
{
	_currentKeys.resize( keyCount );
	_previousKeys.resize( keyCount );

	for ( int i = 0; i < keyCount; i++ ) {
		_currentKeys[ i ] = KeyState::RELEASED;
		_previousKeys[ i ] = KeyState::RELEASED;
	}

	setMouseStartingPosition( Vector2i( 0, 0 ) );
	setNormalizedMouseStartingPosition( Vector2f( 0.0f, 0.0f ) );

	_currentMouseButtons.resize( mouseButtonCount );
	_previousMouseButtons.resize( mouseButtonCount );

	for ( int i = 0; i < mouseButtonCount; i++ ) {
		_currentMouseButtons[ i ] = MouseButtonState::RELEASED;
		_previousMouseButtons[ i ] = MouseButtonState::RELEASED;
	}
}

void InputState::setKeyState( int key, KeyState state )
{
	if ( key < _currentKeys.size() ) {
		_previousKeys[ key ] = _currentKeys[ key ];
		_currentKeys[ key ] = state;
	}
}

void InputState::setMouseStartingPosition( const Vector2i &pos )
{
	_mousePos = pos;
	_mouseDelta = Vector2i( 0, 0 );
}

void InputState::setMousePosition( const Vector2i &pos )
{
	_mouseDelta = pos - _mousePos;
	_mousePos = pos;
}

void InputState::setNormalizedMouseStartingPosition( const Vector2f &pos )
{
	_mousePos = pos;
	_mouseDelta = Vector2f( 0.0f, 0.0f );
}

void InputState::setNormalizedMousePosition( const Vector2f &pos )
{
	_normalizedMouseDelta = pos - _normalizedMousePos;
	_normalizedMousePos = pos;
}

void InputState::setMouseButtonState( int button, MouseButtonState state )
{
	if ( button < _currentMouseButtons.size() ) {
		_previousMouseButtons[ button ] = _currentMouseButtons[ button ];
		_currentMouseButtons[ button ] = state;
	}
}

