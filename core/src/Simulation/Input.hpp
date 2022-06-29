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

#ifndef CRIMILD_SIMULATION_INPUT_
#define CRIMILD_SIMULATION_INPUT_

#include "Foundation/Singleton.hpp"
#include "Mathematics/Vector2.hpp"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

// This codes are ment to match those of GLFW to avoid translation tables
#define CRIMILD_INPUT_KEY_UNKNOWN -1

#define CRIMILD_INPUT_KEY_SPACE 32
#define CRIMILD_INPUT_KEY_APOSTROPHE 39 /* ' */
#define CRIMILD_INPUT_KEY_COMMA 44      /* , */
#define CRIMILD_INPUT_KEY_MINUS 45      /* - */
#define CRIMILD_INPUT_KEY_PERIOD 46     /* . */
#define CRIMILD_INPUT_KEY_SLASH 47      /* / */
#define CRIMILD_INPUT_KEY_0 48
#define CRIMILD_INPUT_KEY_1 49
#define CRIMILD_INPUT_KEY_2 50
#define CRIMILD_INPUT_KEY_3 51
#define CRIMILD_INPUT_KEY_4 52
#define CRIMILD_INPUT_KEY_5 53
#define CRIMILD_INPUT_KEY_6 54
#define CRIMILD_INPUT_KEY_7 55
#define CRIMILD_INPUT_KEY_8 56
#define CRIMILD_INPUT_KEY_9 57
#define CRIMILD_INPUT_KEY_SEMICOLON 59 /* ; */
#define CRIMILD_INPUT_KEY_EQUAL 61     /* = */
#define CRIMILD_INPUT_KEY_A 65
#define CRIMILD_INPUT_KEY_B 66
#define CRIMILD_INPUT_KEY_C 67
#define CRIMILD_INPUT_KEY_D 68
#define CRIMILD_INPUT_KEY_E 69
#define CRIMILD_INPUT_KEY_F 70
#define CRIMILD_INPUT_KEY_G 71
#define CRIMILD_INPUT_KEY_H 72
#define CRIMILD_INPUT_KEY_I 73
#define CRIMILD_INPUT_KEY_J 74
#define CRIMILD_INPUT_KEY_K 75
#define CRIMILD_INPUT_KEY_L 76
#define CRIMILD_INPUT_KEY_M 77
#define CRIMILD_INPUT_KEY_N 78
#define CRIMILD_INPUT_KEY_O 79
#define CRIMILD_INPUT_KEY_P 80
#define CRIMILD_INPUT_KEY_Q 81
#define CRIMILD_INPUT_KEY_R 82
#define CRIMILD_INPUT_KEY_S 83
#define CRIMILD_INPUT_KEY_T 84
#define CRIMILD_INPUT_KEY_U 85
#define CRIMILD_INPUT_KEY_V 86
#define CRIMILD_INPUT_KEY_W 87
#define CRIMILD_INPUT_KEY_X 88
#define CRIMILD_INPUT_KEY_Y 89
#define CRIMILD_INPUT_KEY_Z 90
#define CRIMILD_INPUT_KEY_LEFT_BRACKET 91  /* [ */
#define CRIMILD_INPUT_KEY_BACKSLASH 92     /* \ */
#define CRIMILD_INPUT_KEY_RIGHT_BRACKET 93 /* ] */
#define CRIMILD_INPUT_KEY_GRAVE_ACCENT 96  /* ` */
#define CRIMILD_INPUT_KEY_WORLD_1 161      /* non-US #1 */
#define CRIMILD_INPUT_KEY_WORLD_2 162      /* non-US #2 */

#define CRIMILD_INPUT_KEY_ESCAPE 256
#define CRIMILD_INPUT_KEY_ENTER 257
#define CRIMILD_INPUT_KEY_TAB 258
#define CRIMILD_INPUT_KEY_BACKSPACE 259
#define CRIMILD_INPUT_KEY_INSERT 260
#define CRIMILD_INPUT_KEY_DELETE 261
#define CRIMILD_INPUT_KEY_RIGHT 262
#define CRIMILD_INPUT_KEY_LEFT 263
#define CRIMILD_INPUT_KEY_DOWN 264
#define CRIMILD_INPUT_KEY_UP 265
#define CRIMILD_INPUT_KEY_PAGE_UP 266
#define CRIMILD_INPUT_KEY_PAGE_DOWN 267
#define CRIMILD_INPUT_KEY_HOME 268
#define CRIMILD_INPUT_KEY_END 269
#define CRIMILD_INPUT_KEY_CAPS_LOCK 280
#define CRIMILD_INPUT_KEY_SCROLL_LOCK 281
#define CRIMILD_INPUT_KEY_NUM_LOCK 282
#define CRIMILD_INPUT_KEY_PRINT_SCREEN 283
#define CRIMILD_INPUT_KEY_PAUSE 284
#define CRIMILD_INPUT_KEY_F1 290
#define CRIMILD_INPUT_KEY_F2 291
#define CRIMILD_INPUT_KEY_F3 292
#define CRIMILD_INPUT_KEY_F4 293
#define CRIMILD_INPUT_KEY_F5 294
#define CRIMILD_INPUT_KEY_F6 295
#define CRIMILD_INPUT_KEY_F7 296
#define CRIMILD_INPUT_KEY_F8 297
#define CRIMILD_INPUT_KEY_F9 298
#define CRIMILD_INPUT_KEY_F10 299
#define CRIMILD_INPUT_KEY_F11 300
#define CRIMILD_INPUT_KEY_F12 301
#define CRIMILD_INPUT_KEY_F13 302
#define CRIMILD_INPUT_KEY_F14 303
#define CRIMILD_INPUT_KEY_F15 304
#define CRIMILD_INPUT_KEY_F16 305
#define CRIMILD_INPUT_KEY_F17 306
#define CRIMILD_INPUT_KEY_F18 307
#define CRIMILD_INPUT_KEY_F19 308
#define CRIMILD_INPUT_KEY_F20 309
#define CRIMILD_INPUT_KEY_F21 310
#define CRIMILD_INPUT_KEY_F22 311
#define CRIMILD_INPUT_KEY_F23 312
#define CRIMILD_INPUT_KEY_F24 313
#define CRIMILD_INPUT_KEY_F25 314
#define CRIMILD_INPUT_KEY_KP_0 320
#define CRIMILD_INPUT_KEY_KP_1 321
#define CRIMILD_INPUT_KEY_KP_2 322
#define CRIMILD_INPUT_KEY_KP_3 323
#define CRIMILD_INPUT_KEY_KP_4 324
#define CRIMILD_INPUT_KEY_KP_5 325
#define CRIMILD_INPUT_KEY_KP_6 326
#define CRIMILD_INPUT_KEY_KP_7 327
#define CRIMILD_INPUT_KEY_KP_8 328
#define CRIMILD_INPUT_KEY_KP_9 329
#define CRIMILD_INPUT_KEY_KP_DECIMAL 330
#define CRIMILD_INPUT_KEY_KP_DIVIDE 331
#define CRIMILD_INPUT_KEY_KP_MULTIPLY 332
#define CRIMILD_INPUT_KEY_KP_SUBTRACT 333
#define CRIMILD_INPUT_KEY_KP_ADD 334
#define CRIMILD_INPUT_KEY_KP_ENTER 335
#define CRIMILD_INPUT_KEY_KP_EQUAL 336
#define CRIMILD_INPUT_KEY_LEFT_SHIFT 340
#define CRIMILD_INPUT_KEY_LEFT_CONTROL 341
#define CRIMILD_INPUT_KEY_LEFT_ALT 342
#define CRIMILD_INPUT_KEY_LEFT_SUPER 343
#define CRIMILD_INPUT_KEY_RIGHT_SHIFT 344
#define CRIMILD_INPUT_KEY_RIGHT_CONTROL 345
#define CRIMILD_INPUT_KEY_RIGHT_ALT 346
#define CRIMILD_INPUT_KEY_RIGHT_SUPER 347
#define CRIMILD_INPUT_KEY_MENU 348
#define CRIMILD_INPUT_KEY_LAST CRIMILD_INPUT_KEY_MENU

#define CRIMILD_INPUT_MOD_SHIFT 0x0001
#define CRIMILD_INPUT_MOD_CONTROL 0x0002
#define CRIMILD_INPUT_MOD_ALT 0x0004
#define CRIMILD_INPUT_MOD_SUPER 0x0008

#define CRIMILD_INPUT_MOUSE_BUTTON_1 0
#define CRIMILD_INPUT_MOUSE_BUTTON_2 1
#define CRIMILD_INPUT_MOUSE_BUTTON_3 2
#define CRIMILD_INPUT_MOUSE_BUTTON_4 3
#define CRIMILD_INPUT_MOUSE_BUTTON_5 4
#define CRIMILD_INPUT_MOUSE_BUTTON_6 5
#define CRIMILD_INPUT_MOUSE_BUTTON_7 6
#define CRIMILD_INPUT_MOUSE_BUTTON_8 7
#define CRIMILD_INPUT_MOUSE_BUTTON_LAST CRIMILD_INPUT_MOUSE_BUTTON_8
#define CRIMILD_INPUT_MOUSE_BUTTON_LEFT CRIMILD_INPUT_MOUSE_BUTTON_1
#define CRIMILD_INPUT_MOUSE_BUTTON_RIGHT CRIMILD_INPUT_MOUSE_BUTTON_2
#define CRIMILD_INPUT_MOUSE_BUTTON_MIDDLE CRIMILD_INPUT_MOUSE_BUTTON_3

namespace crimild {

    struct Event;

    class Input : public DynamicSingleton< Input > {
    public:
        enum class MouseCursorMode {
            NORMAL, // default
            HIDDEN,
            GRAB,
        };

    public:
        void handle( const Event & ) noexcept;

        bool isKeyDown( int key ) { return _keys[ key ]; }
        bool isKeyUp( int key ) { return !_keys[ key ]; }

        bool isMouseButtonDown( int button ) { return _mouseButtons[ button ]; }
        bool isMouseButtonUp( int button ) { return !_mouseButtons[ button ]; }

        const Vector2f &getMousePosition( void ) const { return _mousePos; }
        const Vector2f &getMouseDelta( void ) const { return _mouseDelta; }
        const Vector2f &getNormalizedMousePosition( void ) const { return _normalizedMousePos; }
        const Vector2f &getNormalizedMouseDelta( void ) const { return _normalizedMouseDelta; }
        const Vector2f &getMouseScrollDelta( void ) const { return _mouseScrollDelta; }

        MouseCursorMode getMouseCursorMode( void ) const { return _mouseCursorMode; }
        void setMouseCursorMode( MouseCursorMode mode ) { _mouseCursorMode = mode; }

    private:
        std::array< Int8, CRIMILD_INPUT_KEY_LAST > _keys = { 0 };
        std::array< Int8, CRIMILD_INPUT_MOUSE_BUTTON_LAST > _mouseButtons = { 0 };

        Vector2f _mousePos;
        Vector2f _mouseDelta;
        Vector2f _normalizedMousePos;
        Vector2f _normalizedMouseDelta;
        Vector2f _mouseScrollDelta;

        MouseCursorMode _mouseCursorMode;

    public:
        static constexpr const char *AXIS_HORIZONTAL = "Horizontal";
        static constexpr const char *AXIS_VERTICAL = "Vertical";

        void setAxis( std::string key, float value ) { _axes[ key ] = value; }
        float getAxis( std::string key ) { return _axes[ key ]; }

    private:
        std::unordered_map< std::string, float > _axes;

    public:
        bool joystickIsPresent( void ) const { return _joystickAxes.size(); }

        void resetJoystickAxes( std::vector< float > const &axes ) { _joystickAxes = axes; }

        float getJoystickAxis( int axis ) { return _joystickAxes[ axis ]; }

    private:
        std::vector< float > _joystickAxes;
    };

}

#endif
