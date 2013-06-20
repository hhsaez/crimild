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

#ifndef CRIMILD_SIMULATION_INPUT_STATE_
#define CRIMILD_SIMULATION_INPUT_STATE_

#include "Mathematics/Vector.hpp"

#include <vector>

namespace crimild {

	class InputState {
	public:
		static InputState &getCurrentState( void );

		enum class KeyState {
			PRESSED,
			RELEASED
		};

		enum class MouseButtonState {
			PRESSED,
			RELEASED
		};

	private:
		InputState( void );
		~InputState( void );

		InputState( const InputState & ) { }
		InputState &operator=( const InputState & ) { return *this; }

	public:
		void reset( int keyCount, int mouseButtonCount );

		void setKeyState( int key, KeyState state );
		KeyState getCurrentKeyState( int key ) const { return _currentKeys[ key ]; }
		KeyState getPreviousKeyState( int key ) const { return _previousKeys[ key ]; }

		bool isKeyDown( int key ) { return getCurrentKeyState( key ) == KeyState::PRESSED && getPreviousKeyState( key ) == KeyState::RELEASED; }
		bool isKeyStillDown( int key ) { return getCurrentKeyState( key ) == KeyState::PRESSED && getPreviousKeyState( key ) == KeyState::PRESSED; }
		bool isKeyUp( int key ) { return getCurrentKeyState( key ) == KeyState::RELEASED && getPreviousKeyState( key ) == KeyState::PRESSED; }
		bool isKeyStillUp( int key ) { return getCurrentKeyState( key ) == KeyState::RELEASED && getPreviousKeyState( key ) == KeyState::RELEASED; }

		void setMouseStartingPosition( const Vector2i &pos );
		void setMousePosition( const Vector2i &pos );
		const Vector2i &getMousePosition( void ) const { return _mousePos; }
		const Vector2i &getMouseDelta( void ) const { return _mouseDelta; }

		void setNormalizedMouseStartingPosition( const Vector2f &pos );
		void setNormalizedMousePosition( const Vector2f &pos );
		const Vector2f &getNormalizedMousePosition( void ) const { return _normalizedMousePos; }
		const Vector2f &getNormalizedMouseDelta( void ) const { return _normalizedMouseDelta; }

		void setMouseButtonState( int button, MouseButtonState state );
		MouseButtonState getCurrentMouseButtonState( int button ) const { return _currentMouseButtons[ button ]; }
		MouseButtonState getPreviousMouseButtonState( int button ) const { return _previousMouseButtons[ button ]; }

		bool isMouseButtonDown( int button ) { return getCurrentMouseButtonState( button ) == MouseButtonState::PRESSED && getPreviousMouseButtonState( button ) == MouseButtonState::RELEASED; }
		bool isMouseButtonStillDown( int button ) { return getCurrentMouseButtonState( button ) == MouseButtonState::PRESSED && getPreviousMouseButtonState( button ) == MouseButtonState::PRESSED; }
		bool isMouseButtonUp( int button ) { return getCurrentMouseButtonState( button ) == MouseButtonState::RELEASED && getPreviousMouseButtonState( button ) == MouseButtonState::PRESSED; }
		bool isMouseButtonStillUp( int button ) { return getCurrentMouseButtonState( button ) == MouseButtonState::RELEASED && getPreviousMouseButtonState( button ) == MouseButtonState::RELEASED; }

	private:
		std::vector< KeyState > _currentKeys;
		std::vector< KeyState > _previousKeys;

		Vector2i _mousePos;
		Vector2i _mouseDelta;
		Vector2f _normalizedMousePos;
		Vector2f _normalizedMouseDelta;

		std::vector< MouseButtonState > _currentMouseButtons;
		std::vector< MouseButtonState > _previousMouseButtons;
	};

}

#endif

