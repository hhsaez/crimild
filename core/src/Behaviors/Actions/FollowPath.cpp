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
 *     * Neither the name of the copyright holder nor the
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

#include "Behaviors/Actions/FollowPath.hpp"

#include "Components/PathComponent.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::actions;
using namespace crimild::components;

void FollowPath::init( BehaviorContext *context )
{
    Behavior::init( context );

    if ( !context->hasTargets() ) {
        CRIMILD_LOG_WARNING( "No targets set" );
        return;
    }

    m_time = 0;
    m_path = context->getTargetAt( 0 )->getComponent< Path >();
}

Behavior::State FollowPath::step( BehaviorContext *context )
{
    if ( m_path == nullptr ) {
        CRIMILD_LOG_WARNING( "No valid path provided" );
        return Behavior::State::FAILURE;
    }

    const auto world = m_path->evaluate( m_time );
    context->getAgent()->setLocal( world );

    m_time += 0.1 * context->getClock().getDeltaTime();
    if ( m_time > 1 ) {
        return Behavior::State::SUCCESS;
    }

    return Behavior::State::RUNNING;
}

void FollowPath::encode( coding::Encoder &encoder )
{
    Behavior::encode( encoder );
}

void FollowPath::decode( coding::Decoder &decoder )
{
    Behavior::decode( decoder );
}
