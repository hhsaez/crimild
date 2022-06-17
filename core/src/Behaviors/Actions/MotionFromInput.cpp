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

#include "Behaviors/Actions/MotionFromInput.hpp"

#include "Coding/Decoder.cpp"
#include "Coding/Encoder.hpp"
#include "Components/MotionStateComponent.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Input.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

void MotionFromInput::init( BehaviorContext *context )
{
    auto agent = context->getAgent();
    if ( agent == nullptr ) {
        return;
    }

    m_motion = agent->getComponent< MotionState >();
    if ( m_motion == nullptr ) {
        m_motion = agent->attachComponent< MotionState >();
    }
}

Behavior::State MotionFromInput::step( BehaviorContext *context )
{
    auto agent = context->getAgent();
    if ( agent == nullptr ) {
        CRIMILD_LOG_WARNING( "Attempting to use MotionReset behavior without an agent" );
        return Behavior::State::FAILURE;
    }

    if ( m_motion == nullptr ) {
        CRIMILD_LOG_WARNING( "MotionState not initialized" );
        return Behavior::State::FAILURE;
    }

    // Use a reference to make code simpler
    auto &steering = m_motion->steering;

    if ( auto input = Input::getInstance() ) {
        steering.x = m_speed * input->getAxis( Input::AXIS_HORIZONTAL );
        steering.y = m_speed * input->getAxis( Input::AXIS_VERTICAL );
    }

    m_motion->maxForce = m_speed;

    return Behavior::State::SUCCESS;
}

void MotionFromInput::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "speed", m_speed );
}

void MotionFromInput::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "speed", m_speed );
}
