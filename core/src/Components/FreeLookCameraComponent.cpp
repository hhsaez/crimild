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

#include "FreeLookCameraComponent.hpp"

#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/Vector2Ops.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;

void FreeLookCameraComponent::start( void )
{
    _lastMousePos = Vector2::Constants::ZERO;

    registerMessageHandler< crimild::messaging::KeyPressed >(
        [ & ]( crimild::messaging::KeyPressed const &msg ) {
            if ( !isEnabled() ) {
                return;
            }

            float cameraAxisCoeff = 5.0f;

            if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) || Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_RIGHT_SHIFT ) ) {
                cameraAxisCoeff = 20.0f;
            }

            switch ( msg.key ) {
                case 'A':
                case CRIMILD_INPUT_KEY_LEFT:
                    Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, -cameraAxisCoeff );
                    break;

                case 'D':
                case CRIMILD_INPUT_KEY_RIGHT:
                    Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, +cameraAxisCoeff );
                    break;

                case 'W':
                case CRIMILD_INPUT_KEY_UP:
                    Input::getInstance()->setAxis( Input::AXIS_VERTICAL, +cameraAxisCoeff );
                    break;

                case 'S':
                case CRIMILD_INPUT_KEY_DOWN:
                    Input::getInstance()->setAxis( Input::AXIS_VERTICAL, -cameraAxisCoeff );
                    break;

                case 'Q':
                    Input::getInstance()->setAxis( "CameraAxisRoll", -cameraAxisCoeff );
                    break;

                case 'E':
                    Input::getInstance()->setAxis( "CameraAxisRoll", +cameraAxisCoeff );
                    break;
            }
        } );

    registerMessageHandler< crimild::messaging::KeyReleased >(
        [ & ]( crimild::messaging::KeyReleased const &msg ) {
            if ( !isEnabled() ) {
                return;
            }

            switch ( msg.key ) {
                case 'A':
                case 'D':
                case CRIMILD_INPUT_KEY_LEFT:
                case CRIMILD_INPUT_KEY_RIGHT:
                    Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, 0.0f );
                    break;

                case 'W':
                case CRIMILD_INPUT_KEY_UP:
                case 'S':
                case CRIMILD_INPUT_KEY_DOWN:
                    Input::getInstance()->setAxis( Input::AXIS_VERTICAL, 0.0f );
                    break;

                case 'Q':
                case 'E':
                    Input::getInstance()->setAxis( "CameraAxisRoll", 0.0f );
                    break;
            }
        } );

    registerMessageHandler< crimild::messaging::MouseMotion >(
        [ & ]( crimild::messaging::MouseMotion const &msg ) {
            if ( !isEnabled() ) {
                return;
            }

            auto currentPos = Vector2 { msg.nx, msg.ny };
            auto mouseDelta = _initialized ? currentPos - _lastMousePos : Vector2f::Constants::ZERO;
            _initialized = true;
            _lastMousePos = currentPos;

            if ( Input::getInstance()->isMouseButtonDown( CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) ) {
                auto root = getNode();

                auto T = root->getLocal();
                const auto pitch = rotationX( -mouseDelta[ 1 ] );
                const auto U = up( pitch );
                const auto yaw = rotation( U, -mouseDelta[ 0 ] );

                T = pitch * yaw * T;

                root->setLocal( T );
            }
        } );
}

void FreeLookCameraComponent::update( const Clock &c )
{
    if ( Input::getInstance()->isMouseButtonDown( CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) ) {
        Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::GRAB );
    } else {
        Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::NORMAL );
    }

    float dSpeed = getSpeed() * Input::getInstance()->getAxis( Input::AXIS_VERTICAL );
    float rSpeed = getSpeed() * Input::getInstance()->getAxis( Input::AXIS_HORIZONTAL );
    float zSpeed = getSpeed() * Input::getInstance()->getAxis( "CameraAxisRoll" );

    auto root = getNode();

    auto T = root->getLocal();

    // Apply roll first
    //T = rotationZ( c.getDeltaTime() * zSpeed ) * T;

    const auto F = forward( T );
    const auto R = right( T );

    root->setLocal( translation( c.getDeltaTime() * ( dSpeed * F + rSpeed * R ) ) * T );
}
