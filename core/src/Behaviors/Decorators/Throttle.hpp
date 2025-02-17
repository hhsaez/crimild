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

#ifndef CRIMILD_CORE_BEHAVIORS_DECORATORS_THROTTLE_
#define CRIMILD_CORE_BEHAVIORS_DECORATORS_THROTTLE_

#include "Decorator.hpp"

namespace crimild {

    namespace behaviors {

        namespace decorators {

            /**
               \brief Executes the child behavior at most once in the given time frame
             */
            class Throttle : public Decorator {
                CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::decorators::Throttle )

            public:
                inline void setTimeout( Real timeout ) noexcept { m_timeout = timeout; }
                inline Real getTimeout( void ) const noexcept { return m_timeout; }

                virtual Behavior::State step( BehaviorContext *context ) noexcept override
                {
                    m_clock.tick();
                    if ( m_clock.getAccumTime() >= m_timeout ) {
                        m_clock.reset();
                        return Decorator::step( context );
                    }
                    return Behavior::State::RUNNING;
                }

            private:
                Real m_timeout = 0;
                Clock m_clock;
            };

            [[nodiscard]] inline SharedPointer< Throttle > throttle( BehaviorPtr behavior, Real timeout ) noexcept
            {
                auto t = crimild::alloc< Throttle >();
                t->setTimeout( timeout );
                t->setBehavior( behavior );
                return t;
            }

        }

    }

}

#endif
