/*
 * Copyright (c) 2013 - 2018, Hugo Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Crimild nor the
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

#include "SDLEventSystem.hpp"
#include "WindowSystem.hpp"

#include <Concurrency/Async.hpp>
#include <Foundation/Profiler.hpp>
#include <Simulation/Simulation.hpp>
#include <Simulation/Input.hpp>
#include <Simulation/Console/Console.hpp>

#include <SDL.h>

using namespace crimild;
using namespace crimild::messaging;
using namespace crimild::sdl;

SDLEventSystem::SDLEventSystem( void )
{
    registerMessageHandler< messaging::WindowSystemDidCreateWindow >( [ this ]( messaging::WindowSystemDidCreateWindow const &m ) {
		auto window = m.window;
		SDL_GetWindowSize( window, &_windowSize[ 0 ], &_windowSize[ 1 ] );
		
		crimild::concurrency::sync_frame( std::bind( &SDLEventSystem::update, this ) );
    });
}

SDLEventSystem::~SDLEventSystem( void )
{

}

bool SDLEventSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}

	_keycodes[ SDLK_UNKNOWN ] = CRIMILD_INPUT_KEY_UNKNOWN;
	_keycodes[ SDLK_BACKSPACE ] = CRIMILD_INPUT_KEY_BACKSPACE;
	_keycodes[ SDLK_TAB ] = CRIMILD_INPUT_KEY_TAB;
	_keycodes[ SDLK_RETURN ] = CRIMILD_INPUT_KEY_ENTER;
	_keycodes[ SDLK_ESCAPE ] = CRIMILD_INPUT_KEY_ESCAPE;
	_keycodes[ SDLK_SPACE ] = CRIMILD_INPUT_KEY_SPACE;
	//_keycodes[ SDLK_EXCLAIM ] = CRIMILD_INPUT_KEY_EXCLAIM;
	//_keycodes[ SDLK_QUOTEDBL ] = CRIMILD_INPUT_KEY_QUOTEDBL;
	//_keycodes[ SDLK_HASH ] = CRIMILD_INPUT_KEY_HASH;
	//_keycodes[ SDLK_DOLAR ] = CRIMILD_INPUT_KEY_DOLAR;
	//_keycodes[ SDLK_PERCENT ] = CRIMILD_INPUT_KEY_PERCENT;
	//_keycodes[ SDLK_AMPERSAND ] = CRIMILD_INPUT_KEY_AMPERSAND;
	//_keycodes[ SDLK_QUOTE ] = CRIMILD_INPUT_KEY_QUOTE;
	//_keycodes[ SDLK_LEFTPAREN ] = CRIMILD_INPUT_KEY_LEFTPAREN;
	//_keycodes[ SDLK_RIGHTPAREN ] = CRIMILD_INPUT_KEY_RIGHTPAREN;
	//_keycodes[ SDLK_ASTERISK ] = CRIMILD_INPUT_KEY_ASTERISK;
	//_keycodes[ SDLK_PLUS ] = CRIMILD_INPUT_KEY_PLUS;
	_keycodes[ SDLK_COMMA ] = CRIMILD_INPUT_KEY_COMMA;
	_keycodes[ SDLK_MINUS ] = CRIMILD_INPUT_KEY_MINUS;
	_keycodes[ SDLK_PERIOD ] = CRIMILD_INPUT_KEY_PERIOD;
	_keycodes[ SDLK_SLASH ] = CRIMILD_INPUT_KEY_SLASH;

	_keycodes[ SDLK_0 ] = CRIMILD_INPUT_KEY_0;
	_keycodes[ SDLK_1 ] = CRIMILD_INPUT_KEY_1;
	_keycodes[ SDLK_2 ] = CRIMILD_INPUT_KEY_2;
	_keycodes[ SDLK_3 ] = CRIMILD_INPUT_KEY_3;
	_keycodes[ SDLK_4 ] = CRIMILD_INPUT_KEY_4;
	_keycodes[ SDLK_5 ] = CRIMILD_INPUT_KEY_5;
	_keycodes[ SDLK_6 ] = CRIMILD_INPUT_KEY_6;
	_keycodes[ SDLK_7 ] = CRIMILD_INPUT_KEY_7;
	_keycodes[ SDLK_8 ] = CRIMILD_INPUT_KEY_8;
	_keycodes[ SDLK_9 ] = CRIMILD_INPUT_KEY_9;

	//_keycodes[ SDLK_COLON ] = CRIMILD_INPUT_KEY_COLON;
	_keycodes[ SDLK_SEMICOLON ] = CRIMILD_INPUT_KEY_SEMICOLON;
	//_keycodes[ SDLK_LESS ] = CRIMILD_INPUT_KEY_LESS;
	//_keycodes[ SDLK_EQUAL ] = CRIMILD_INPUT_KEY_EQUAL;
	//_keycodes[ SDLK_GREATER ] = CRIMILD_INPUT_KEY_GREATER;
	//_keycodes[ SDLK_QUESTION ] = CRIMILD_INPUT_KEY_QUESTION;
	//_keycodes[ SDLK_AT ] = CRIMILD_INPUT_KEY_AT;
	_keycodes[ SDLK_LEFTBRACKET ] = CRIMILD_INPUT_KEY_LEFT_BRACKET;
	_keycodes[ SDLK_BACKSLASH ] = CRIMILD_INPUT_KEY_BACKSLASH;
	_keycodes[ SDLK_RIGHTBRACKET ] = CRIMILD_INPUT_KEY_RIGHT_BRACKET;
	//_keycodes[ SDLK_CARET ] = CRIMILD_INPUT_KEY_CARET;
	//_keycodes[ SDLK_UNDERSCORE ] = CRIMILD_INPUT_KEY_UNDERSCORE;
	_keycodes[ SDLK_BACKQUOTE ] = CRIMILD_INPUT_KEY_GRAVE_ACCENT;
	
	_keycodes[ SDLK_a ] = CRIMILD_INPUT_KEY_A;
	_keycodes[ SDLK_b ] = CRIMILD_INPUT_KEY_B;
	_keycodes[ SDLK_c ] = CRIMILD_INPUT_KEY_C;
	_keycodes[ SDLK_d ] = CRIMILD_INPUT_KEY_D;
	_keycodes[ SDLK_e ] = CRIMILD_INPUT_KEY_E;
	_keycodes[ SDLK_f ] = CRIMILD_INPUT_KEY_F;
	_keycodes[ SDLK_g ] = CRIMILD_INPUT_KEY_G;
	_keycodes[ SDLK_h ] = CRIMILD_INPUT_KEY_H;
	_keycodes[ SDLK_i ] = CRIMILD_INPUT_KEY_I;
	_keycodes[ SDLK_j ] = CRIMILD_INPUT_KEY_J;
	_keycodes[ SDLK_k ] = CRIMILD_INPUT_KEY_K;
	_keycodes[ SDLK_l ] = CRIMILD_INPUT_KEY_L;
	_keycodes[ SDLK_m ] = CRIMILD_INPUT_KEY_M;
	_keycodes[ SDLK_n ] = CRIMILD_INPUT_KEY_N;
	_keycodes[ SDLK_o ] = CRIMILD_INPUT_KEY_O;
	_keycodes[ SDLK_p ] = CRIMILD_INPUT_KEY_P;
	_keycodes[ SDLK_q ] = CRIMILD_INPUT_KEY_Q;
	_keycodes[ SDLK_r ] = CRIMILD_INPUT_KEY_R;
	_keycodes[ SDLK_s ] = CRIMILD_INPUT_KEY_S;
	_keycodes[ SDLK_t ] = CRIMILD_INPUT_KEY_T;
	_keycodes[ SDLK_u ] = CRIMILD_INPUT_KEY_U;
	_keycodes[ SDLK_v ] = CRIMILD_INPUT_KEY_V;
	_keycodes[ SDLK_w ] = CRIMILD_INPUT_KEY_W;
	_keycodes[ SDLK_x ] = CRIMILD_INPUT_KEY_X;
	_keycodes[ SDLK_y ] = CRIMILD_INPUT_KEY_Y;
	_keycodes[ SDLK_z ] = CRIMILD_INPUT_KEY_Z;
	
	_keycodes[ SDLK_DELETE ] = CRIMILD_INPUT_KEY_DELETE;
	_keycodes[ SDLK_CAPSLOCK ] = CRIMILD_INPUT_KEY_CAPS_LOCK;

	_keycodes[ SDLK_F1 ] = CRIMILD_INPUT_KEY_F1;
	_keycodes[ SDLK_F2 ] = CRIMILD_INPUT_KEY_F2;
	_keycodes[ SDLK_F3 ] = CRIMILD_INPUT_KEY_F3;
	_keycodes[ SDLK_F4 ] = CRIMILD_INPUT_KEY_F4;
	_keycodes[ SDLK_F5 ] = CRIMILD_INPUT_KEY_F5;
	_keycodes[ SDLK_F6 ] = CRIMILD_INPUT_KEY_F6;
	_keycodes[ SDLK_F7 ] = CRIMILD_INPUT_KEY_F7;
	_keycodes[ SDLK_F8 ] = CRIMILD_INPUT_KEY_F8;
	_keycodes[ SDLK_F9 ] = CRIMILD_INPUT_KEY_F9;
	_keycodes[ SDLK_F10 ] = CRIMILD_INPUT_KEY_F10;
	_keycodes[ SDLK_F11 ] = CRIMILD_INPUT_KEY_F11;
	_keycodes[ SDLK_F12 ] = CRIMILD_INPUT_KEY_F12;
	_keycodes[ SDLK_F13 ] = CRIMILD_INPUT_KEY_F13;
	_keycodes[ SDLK_F14 ] = CRIMILD_INPUT_KEY_F14;
	_keycodes[ SDLK_F15 ] = CRIMILD_INPUT_KEY_F15;
	_keycodes[ SDLK_F16 ] = CRIMILD_INPUT_KEY_F16;
	_keycodes[ SDLK_F17 ] = CRIMILD_INPUT_KEY_F17;
	_keycodes[ SDLK_F18 ] = CRIMILD_INPUT_KEY_F18;
	_keycodes[ SDLK_F19 ] = CRIMILD_INPUT_KEY_F19;
	_keycodes[ SDLK_F20 ] = CRIMILD_INPUT_KEY_F20;
	_keycodes[ SDLK_F21 ] = CRIMILD_INPUT_KEY_F21;
	_keycodes[ SDLK_F22 ] = CRIMILD_INPUT_KEY_F22;
	_keycodes[ SDLK_F23 ] = CRIMILD_INPUT_KEY_F23;
	_keycodes[ SDLK_F24 ] = CRIMILD_INPUT_KEY_F24;
	
	_keycodes[ SDLK_PRINTSCREEN ] = CRIMILD_INPUT_KEY_PRINT_SCREEN;
	_keycodes[ SDLK_SCROLLLOCK ] = CRIMILD_INPUT_KEY_SCROLL_LOCK;
	_keycodes[ SDLK_PAUSE ] = CRIMILD_INPUT_KEY_PAUSE;
	_keycodes[ SDLK_INSERT ] = CRIMILD_INPUT_KEY_INSERT;
	_keycodes[ SDLK_HOME ] = CRIMILD_INPUT_KEY_HOME;
	_keycodes[ SDLK_PAGEUP ] = CRIMILD_INPUT_KEY_PAGE_UP;
	_keycodes[ SDLK_END ] = CRIMILD_INPUT_KEY_END;
	_keycodes[ SDLK_PAGEDOWN ] = CRIMILD_INPUT_KEY_PAGE_DOWN;

	_keycodes[ SDLK_RIGHT ] = CRIMILD_INPUT_KEY_RIGHT;
	_keycodes[ SDLK_DOWN ] = CRIMILD_INPUT_KEY_DOWN;
	_keycodes[ SDLK_LEFT ] = CRIMILD_INPUT_KEY_LEFT;
	_keycodes[ SDLK_UP ] = CRIMILD_INPUT_KEY_UP;

	_keycodes[ SDLK_NUMLOCKCLEAR ] = CRIMILD_INPUT_KEY_NUM_LOCK;

	_keycodes[ SDLK_KP_MULTIPLY ] = CRIMILD_INPUT_KEY_KP_MULTIPLY;
	_keycodes[ SDLK_KP_MINUS ] = CRIMILD_INPUT_KEY_KP_SUBTRACT;
	_keycodes[ SDLK_KP_PLUS ] = CRIMILD_INPUT_KEY_KP_ADD;
	_keycodes[ SDLK_KP_ENTER ] = CRIMILD_INPUT_KEY_KP_ENTER;
	_keycodes[ SDLK_PERIOD ] = CRIMILD_INPUT_KEY_KP_DECIMAL;

	_keycodes[ SDLK_0 ] = CRIMILD_INPUT_KEY_KP_0;
	_keycodes[ SDLK_1 ] = CRIMILD_INPUT_KEY_KP_1;
	_keycodes[ SDLK_2 ] = CRIMILD_INPUT_KEY_KP_2;
	_keycodes[ SDLK_3 ] = CRIMILD_INPUT_KEY_KP_3;
	_keycodes[ SDLK_4 ] = CRIMILD_INPUT_KEY_KP_4;
	_keycodes[ SDLK_5 ] = CRIMILD_INPUT_KEY_KP_5;
	_keycodes[ SDLK_6 ] = CRIMILD_INPUT_KEY_KP_6;
	_keycodes[ SDLK_7 ] = CRIMILD_INPUT_KEY_KP_7;
	_keycodes[ SDLK_8 ] = CRIMILD_INPUT_KEY_KP_8;
	_keycodes[ SDLK_9 ] = CRIMILD_INPUT_KEY_KP_9;

	_keycodes[ SDLK_KP_DIVIDE ] = CRIMILD_INPUT_KEY_KP_DIVIDE;
	_keycodes[ SDLK_KP_EQUALS ] = CRIMILD_INPUT_KEY_KP_EQUAL;

	_keycodes[ SDLK_MENU ] = CRIMILD_INPUT_KEY_MENU;
	
	_keycodes[ SDLK_LSHIFT ] = CRIMILD_INPUT_KEY_LEFT_SHIFT;
	_keycodes[ SDLK_LCTRL ] = CRIMILD_INPUT_KEY_LEFT_CONTROL;
	_keycodes[ SDLK_LALT ] = CRIMILD_INPUT_KEY_LEFT_ALT;
	_keycodes[ SDLK_LGUI ] = CRIMILD_INPUT_KEY_LEFT_SUPER;
	_keycodes[ SDLK_RSHIFT ] = CRIMILD_INPUT_KEY_RIGHT_SHIFT;
	_keycodes[ SDLK_RCTRL ] = CRIMILD_INPUT_KEY_RIGHT_CONTROL;
	_keycodes[ SDLK_RALT ] = CRIMILD_INPUT_KEY_RIGHT_ALT;
	_keycodes[ SDLK_RGUI ] = CRIMILD_INPUT_KEY_RIGHT_SUPER;
	
	//_keycodes[ SDLK_ ] = CRIMILD_INPUT_MOD_SHIFT;
	//_keycodes[ SDLK_ ] = CRIMILD_INPUT_MOD_CONTROL;
	//_keycodes[ SDLK_ ] = CRIMILD_INPUT_MOD_ALT;
	//_keycodes[ SDLK_ ] = CRIMILD_INPUT_MOD_SUPER;
	
	_mousecodes[ SDL_BUTTON_LEFT ] = CRIMILD_INPUT_MOUSE_BUTTON_LEFT;
	_mousecodes[ SDL_BUTTON_MIDDLE ] = CRIMILD_INPUT_MOUSE_BUTTON_MIDDLE;
	_mousecodes[ SDL_BUTTON_RIGHT ] = CRIMILD_INPUT_MOUSE_BUTTON_RIGHT;
	
	return true;
}

void SDLEventSystem::update( void )
{
    CRIMILD_PROFILE( "Update Input" )

	SDL_Event event;
	while ( SDL_PollEvent( &event ) ) {
		switch( event.type ) {
			case SDL_QUIT:
				crimild::concurrency::sync_frame( [] {
					Simulation::getInstance()->stop();
				});
				return;

			case SDL_MOUSEBUTTONDOWN: {
				MessageQueue::getInstance()->pushMessage( MouseButtonDown { _mousecodes[ event.button.button ] } );
				break;
			}

			case SDL_MOUSEBUTTONUP: {
				MessageQueue::getInstance()->pushMessage( MouseButtonUp { _mousecodes[ event.button.button ] } );
				break;
			}

			case SDL_MOUSEWHEEL: {
				MessageQueue::getInstance()->pushMessage( MouseScroll { ( crimild::Real32 ) event.wheel.x, ( crimild::Real32 ) event.wheel.y } );
				break;
			}

			case SDL_KEYDOWN: {
				if ( event.key.keysym.sym != SDLK_UNKNOWN ) {
					auto key = _keycodes[ event.key.keysym.sym ];

					if ( Console::getInstance()->isEnabled() ) {
						switch ( key ) {
							case CRIMILD_INPUT_KEY_BACKSPACE:
							case CRIMILD_INPUT_KEY_UP:
							case CRIMILD_INPUT_KEY_DOWN:
							case CRIMILD_INPUT_KEY_ENTER:
								Console::getInstance()->handleInput( key, 0 );
								break;
								
							default:
								break;
						}
					}

					if ( !Console::getInstance()->isActive() ) {
						MessageQueue::getInstance()->pushMessage( KeyPressed { key } );
					}
				}

				break;
			}

			case SDL_KEYUP: {
				if ( !Console::getInstance()->isActive() ) {
					MessageQueue::getInstance()->pushMessage( KeyReleased { _keycodes[ event.key.keysym.sym ] } );
				}
				break;
			}

			case SDL_TEXTINPUT: {
				if ( Console::getInstance()->isEnabled() ) {
					auto text = std::string( event.text.text );
					for ( auto c : text ) {
						Console::getInstance()->handleInput( c, 0 );
					}
				}
				break;
			}
				
			default:
				break;
		}
	}

    // trigger MouseMotion in every update to handle cases
    // when the mouse is not moving and the delta pos should
    // be updated 
	int mouseX, mouseY;
	SDL_GetMouseState( &mouseX, &mouseY );
	MessageQueue::getInstance()->pushMessage( MouseMotion {
		( crimild::Real32 ) mouseX,
		( crimild::Real32 ) mouseY,
		( crimild::Real32 ) mouseX / ( crimild::Real32 ) _windowSize.x(),
		( crimild::Real32 ) mouseY / ( crimild::Real32 ) _windowSize.y()
	});

    crimild::concurrency::sync_frame( std::bind( &SDLEventSystem::update, this ) );
}

void SDLEventSystem::stop( void )
{
	System::stop();
}

