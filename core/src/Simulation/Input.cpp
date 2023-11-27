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

#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector2Ops.hpp"
#include "Rendering/Renderer.hpp"
#include "Simulation/Event.hpp"

using namespace crimild;

void Input::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::KEY_DOWN: {
            _keys[ e.keyboard.key ] = 1;

            switch ( e.keyboard.key ) {
                case CRIMILD_INPUT_KEY_A:
                case CRIMILD_INPUT_KEY_LEFT:
                    setAxis( Input::AXIS_HORIZONTAL, -1 );
                    break;

                case CRIMILD_INPUT_KEY_D:
                case CRIMILD_INPUT_KEY_RIGHT:
                    setAxis( Input::AXIS_HORIZONTAL, +1 );
                    break;

                case CRIMILD_INPUT_KEY_W:
                case CRIMILD_INPUT_KEY_UP:
                    setAxis( Input::AXIS_VERTICAL, +1 );
                    break;

                case CRIMILD_INPUT_KEY_S:
                case CRIMILD_INPUT_KEY_DOWN:
                    setAxis( Input::AXIS_VERTICAL, -1 );
                    break;

                default:
                    break;
            }
            break;
        }

        case Event::Type::KEY_UP: {
            _keys[ e.keyboard.key ] = 0;

            switch ( e.keyboard.key ) {
                case CRIMILD_INPUT_KEY_A:
                case CRIMILD_INPUT_KEY_D:
                case CRIMILD_INPUT_KEY_LEFT:
                case CRIMILD_INPUT_KEY_RIGHT:
                    setAxis( Input::AXIS_HORIZONTAL, 0 );
                    break;

                case CRIMILD_INPUT_KEY_W:
                case CRIMILD_INPUT_KEY_S:
                case CRIMILD_INPUT_KEY_UP:
                case CRIMILD_INPUT_KEY_DOWN:
                    setAxis( Input::AXIS_VERTICAL, 0 );
                    break;

                default:
                    break;
            }
            break;
        }

        case Event::Type::MOUSE_MOTION: {
            const auto pos = Vector2 { Real( e.motion.pos.x ), Real( e.motion.pos.y ) };
            _mouseDelta = pos - _mousePos;
            _mousePos = pos;
            break;
        }

        case Event::Type::MOUSE_BUTTON_DOWN: {
            _mouseButtons[ e.button.button ] = 1;
            break;
        }

        case Event::Type::MOUSE_BUTTON_UP: {
            _mouseButtons[ e.button.button ] = 0;
            break;
        }

        default:
            break;
    }
}
