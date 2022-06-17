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

#ifndef CRIMILD_CORE_COMPONENTS_MOTION_STATE_
#define CRIMILD_CORE_COMPONENTS_MOTION_STATE_

#include "Components/NodeComponent.hpp"
#include "Mathematics/Point3_constants.hpp"
#include "Mathematics/Vector3_constants.hpp"

namespace crimild {

    /**
     * \brief State for motion behaviors
     *
     * This component stores the state required for motion behaviors to work
     * correctly.
     *
     * \see crimild::behaviors::actions::MotionReset
     * \see crimild::behaviors::actions::MotionApply
     */
    class MotionState : public NodeComponent {
        CRIMILD_IMPLEMENT_RTTI( crimild::MotionState )

    public:
        /**
         * \brief Current velocity
         *
         * This value is updated by crimild::behaviors::actions::MotionApply
         */
        Vector3 velocity = Vector3::Constants::ZERO;

        /**
         * \brief Current position
         *
         * This value is set by crimild::behaviors::actions::MotionReset to be
         * the same as the Node's current location.
         *
         * It is updated by crimild::behaviors::actions::MotionApply based on
         * current velocity and steering force
         */
        Point3 position = Point3::Constants::ZERO;

        /**
         * \brief Steering force
         *
         * This value is reset by crimild::behaviors::actions::MotionReset on
         * every step.
         */
        Vector3 steering = Vector3::Constants::ZERO;

        float maxVelocity = 1.0f;
        float maxForce = 1.0f;
        float mass = 0.0f;
        float slowingRadius = 1.0f;

        /**
            \name Coding
         */
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

}

#endif
