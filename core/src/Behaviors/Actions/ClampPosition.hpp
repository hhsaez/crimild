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

#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_CLAMP_POSITION_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_CLAMP_POSITION_

#include "Behaviors/Behavior.hpp"
#include "Crimild_Mathematics.hpp"

namespace crimild {

    namespace behaviors {

        namespace actions {

            /**
             * \brief Ensures an agent's position is always within limits
             */
            class ClampPosition : public Behavior {
                CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::actions::ClampPosition )

            public:
                virtual crimild::behaviors::Behavior::State step( crimild::behaviors::BehaviorContext *context ) override;

                inline void setLimits( const Bounds3 &limits ) noexcept { m_limits = limits; }
                inline const Bounds3 &getLimits( void ) const noexcept { return m_limits; }

            private:
                Bounds3 m_limits = {
                    .min = Point3f { -1, -1, -1 },
                    .max = Point3f { 1, 1, 1 },
                };

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
