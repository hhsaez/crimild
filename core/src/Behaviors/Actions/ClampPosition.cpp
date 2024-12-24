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

#include "Behaviors/Actions/ClampPosition.hpp"

#include "Crimild_Coding.hpp"
#include "Crimild_Mathematics.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Simulation.hpp"

namespace crimild {

    [[nodiscard]] inline constexpr Transformation translation( const Point3f &P ) noexcept
    {
        return translation( Vector3 { P.x, P.y, P.z } );
    }

}

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

Behavior::State ClampPosition::step( BehaviorContext *context )
{
    auto agent = context->getAgent();
    if ( agent == nullptr ) {
        return Behavior::State::FAILURE;
    }

    auto P = origin( agent->getLocal() );
    P = clamp( P, m_limits.min, m_limits.max );
    agent->setLocal( translation( P ) );

    return Behavior::State::SUCCESS;
}

void ClampPosition::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );

    encoder.encode( "limits_min", m_limits.min );
    encoder.encode( "limits_max", m_limits.max );
}

void ClampPosition::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );

    decoder.decode( "limits_min", m_limits.min );
    decoder.decode( "limits_max", m_limits.max );
}
