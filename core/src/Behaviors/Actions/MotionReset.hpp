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

#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_RESET_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_RESET_

#include "Behaviors/Behavior.hpp"

namespace crimild {

    class MotionState;

    namespace behaviors {

        namespace actions {

            /**
             * \brief Resets motion state and create default values if needed.
             *
             * During initialization, this behavior will fetch the current motion
             * state (position, velocity and steering) from the agent or attach one
             * if none is present. Therefore, this behavior is usually the first
             * one in the a sequence of motion behaviors.
             *
             * During each step, the agent's position is copied to the corresponding
             * field in motion state. Steering is reset too. Velocity is **not**
             * changed, though.
             */
            class MotionReset : public Behavior {
                CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::actions::MotionReset )

            public:
                virtual void init( crimild::behaviors::BehaviorContext *context ) override;
                virtual crimild::behaviors::Behavior::State step( crimild::behaviors::BehaviorContext *context ) override;

            private:
                MotionState *m_motion = nullptr;

                /**
                   \name Coding support
                */
                //@{

            public:
                virtual void encode( coding::Encoder &encoder ) override;
                virtual void decode( coding::Decoder &decoder ) override;

                //@}
            };

        }

    }

}

#endif
