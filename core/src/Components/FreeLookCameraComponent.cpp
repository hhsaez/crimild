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

#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_euler.hpp"
#include "Mathematics/Transformation_inverse.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/Vector2Ops.hpp"
#include "Mathematics/Vector2_constants.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Mathematics/normalize.hpp"
#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;

void FreeLookCameraComponent::start( void )
{
    _lastMousePos = Vector2::Constants::ZERO;

    m_position = location( getNode()->getLocal() );

    extractYawPitchRoll( getNode()->getLocal(), m_yaw, m_pitch, m_roll );
}

void FreeLookCameraComponent::update( const Clock &c )
{
    if ( Input::getInstance()->isMouseButtonDown( getMouseLookButton() ) ) {
        Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::GRAB );
    } else {
        Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::NORMAL );
    }

    auto speedCoeff = 1.0f;
    if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) || Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_RIGHT_SHIFT ) ) {
        speedCoeff = 20.0f;
    }

    float dSpeed = speedCoeff * getSpeed() * Input::getInstance()->getAxis( Input::AXIS_VERTICAL );
    float rSpeed = speedCoeff * getSpeed() * Input::getInstance()->getAxis( Input::AXIS_HORIZONTAL );

    float zSpeed = getSpeed() * Input::getInstance()->getAxis( "CameraAxisRoll" );
    m_roll += 0.005f * zSpeed;

    auto root = getNode();

    const auto mousePos = Input::getInstance()->getMousePosition();
    const auto mouseDelta = _initialized ? mousePos - _lastMousePos : Vector2::Constants::ZERO;
    _initialized = true;
    _lastMousePos = mousePos;

    if ( Input::getInstance()->isMouseButtonDown( getMouseLookButton() ) ) {
        m_pitch -= 0.005f * mouseDelta[ 1 ];
        m_yaw -= 0.005f * mouseDelta[ 0 ];
    }

    const auto E = euler( m_yaw, m_pitch, m_roll );

    const auto F = forward( E );
    const auto R = right( E );

    m_position = m_position + c.getDeltaTime() * ( dSpeed * F + rSpeed * R );
    const auto T = translation( vector3( m_position ) );

    root->setLocal( T * E );
}
