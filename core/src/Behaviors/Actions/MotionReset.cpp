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

#include "MotionReset.hpp"

#include "Components/MotionStateComponent.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Vector3.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;

void MotionReset::init( BehaviorContext *context )
{
    Behavior::init( context );

    auto agent = context->getAgent();
    if ( agent == nullptr ) {
        return;
    }

    m_motion = agent->getComponent< MotionState >();
    if ( m_motion == nullptr ) {
        m_motion = agent->attachComponent< MotionState >();
    }
}

Behavior::State MotionReset::step( BehaviorContext *context )
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

    m_motion->position = location( agent->getLocal() );
    m_motion->steering = Vector3f::ZERO;

    // if ( context->hasTargets() ) {
    //     auto target = context->getTargetAt( 0 );
    //     const auto targetLocation = location( target->getLocal() );
    //     context->setValue( "motion.target", targetLocation );
    // } else {
    //     // set target to self so no motion will be applied
    //     context->setValue( "motion.target", agentLocation );
    // }

    return Behavior::State::SUCCESS;
}

void MotionReset::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );
}

void MotionReset::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );
}
