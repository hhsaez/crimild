/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holders nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_CORE_SIMULATION_EVENT_
#define CRIMILD_CORE_SIMULATION_EVENT_

#include "Foundation/Types.hpp"
#include "Rendering/Extent.hpp"
#include "Simulation/Input.hpp"

#include <iostream>

namespace crimild {

    class Node;

    /**
     * \brief Keyboard button event information
     */
    struct Keyboard {
        enum class State {
            PRESSED,
            REPEAT,
            RELEASED,
        };

        State state = State::PRESSED;
        UInt32 key = CRIMILD_INPUT_KEY_UNKNOWN;
        UInt32 scancode = 0;
        UInt32 mod = 0;
    };

    struct MouseButton {
        enum class State {
            PRESSED,
            RELEASED,
        };

        UInt8 button = CRIMILD_INPUT_MOUSE_BUTTON_LEFT;
        State state = State::PRESSED;
        Vector2i pos = Vector2i { 0, 0 };

        // Normalized mouse position in range [0, 1]
        Vector2f npos = Vector2f { 0, 0 };
    };

    struct MouseMotion {
        Vector2i pos = Vector2i { 0, 0 };

        // Normalized mouse position in range [0, 1]
        Vector2f npos = Vector2f { 0, 0 };
    };

    /**
     * \brief Mouse wheel event information
     */
    struct MouseWheel {
        // Horizontal scroll offset
        Int32 x;
        // Vertical scroll offset
        Int32 y;
    };

    /**
     * \brief Text input event information
     */
    struct TextInput {
        UInt32 codepoint;
    };

    struct Event {
        enum class Type {
            NONE,

            START,
            TICK,
            RENDER,
            TERMINATE,
            STOP,

            WINDOW_RESIZE,
            WINDOW_CLOSED,

            KEY_DOWN,
            KEY_REPEAT,
            KEY_UP,

            MOUSE_MOTION,
            MOUSE_BUTTON_DOWN,
            MOUSE_BUTTON_UP,
            MOUSE_CLICK,
            MOUSE_WHEEL,

            TEXT,

            NODE_SELECTED,

            SCENE_CHANGED,

            SIMULATION_START,
            SIMULATION_UPDATE,
            SIMULATION_RENDER,
            SIMULATION_STOP,
        };

        Type type = Type::NONE;
        UInt64 timestamp = 0;

        union {
            Extent2D extent = Extent2D {};
            Keyboard keyboard;
            MouseButton button;
            MouseMotion motion;
            MouseWheel wheel;
            TextInput text;
            Node *node;
        };
    };

    static std::ostream &operator<<( std::ostream &out, const Event &e )
    {
        switch ( e.type ) {
            case Event::Type::NONE:
                out << "NONE";
                break;
            case Event::Type::START:
                out << "START";
                break;
            case Event::Type::TICK:
                out << "TICK";
                break;
            case Event::Type::STOP:
                out << "STOP";
                break;
            case Event::Type::WINDOW_RESIZE:
                out << "WINDOW_RESIZE";
                break;
            case Event::Type::KEY_DOWN:
                out << "KEY_DOWN";
                break;
            case Event::Type::KEY_REPEAT:
                out << "KEY_REPEAT";
                break;
            case Event::Type::KEY_UP:
                out << "KEY_UP";
                break;
            case Event::Type::MOUSE_MOTION:
                out << "MOUSE_MOTION";
                break;
            case Event::Type::MOUSE_BUTTON_DOWN:
                out << "MOUSE_BUTTON_DOWN";
                break;
            case Event::Type::MOUSE_BUTTON_UP:
                out << "MOUSE_BUTTON_UP";
                break;
            case Event::Type::MOUSE_CLICK:
                out << "MOUSE_CLICK";
                break;
            case Event::Type::MOUSE_WHEEL:
                out << "MOUSE_WHEEL";
                break;
            case Event::Type::TEXT:
                out << "TEXT";
                break;
            case Event::Type::NODE_SELECTED:
                out << "NODE_SELECTED";
                break;
            case Event::Type::SIMULATION_START:
                out << "SIMULATION_START";
                break;
            case Event::Type::SIMULATION_UPDATE:
                out << "SIMULATION_UPDATE";
                break;
            case Event::Type::SIMULATION_RENDER:
                out << "SIMULATION_RENDER";
                break;
            case Event::Type::SIMULATION_STOP:
                out << "SIMULATION_STOP";
                break;
        }

        return out;
    }

}

#endif
