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

#include "Behaviors/Actions/Rotate.hpp"

#include "Crimild_Coding.hpp"
#include "Crimild_Mathematics.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

Rotate::Rotate( const Vector3f &axis, Radians angle ) noexcept
    : m_axis( axis ),
      m_angle( angle )
{
}

void Rotate::init( BehaviorContext *context ) noexcept
{
    Behavior::init( context );
}

Behavior::State Rotate::step( BehaviorContext *context ) noexcept
{
    m_clock.tick();
    const auto dt = m_clock.getDeltaTime();

    auto agent = context->getAgent();
    agent->setLocal( agent->getLocal()( rotation( m_axis, dt * m_angle ) ) );

    return Behavior::State::RUNNING;
}

void Rotate::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "axis", m_axis );
    encoder.encode( "angle", m_angle );
}

void Rotate::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "axis", m_axis );
    decoder.decode( "angle", m_angle );
}
