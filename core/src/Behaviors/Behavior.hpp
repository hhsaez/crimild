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

#ifndef CRIMILD_CORE_BEHAVIORS_BEHAVIOR_
#define CRIMILD_CORE_BEHAVIORS_BEHAVIOR_

#include "BehaviorContext.hpp"
#include "Coding/Codable.hpp"
#include "Crimild_Foundation.hpp"
#include "Messaging/MessageQueue.hpp"

namespace crimild {

    namespace messaging {

        /**
           \brief A simple struct for sending messages between behaviors
         */
        struct BehaviorEvent {
            std::string name;
        };

    }

    namespace behaviors {

        class Behavior;

        /**
           \brief Base class for behaviors
        */
        class Behavior : public coding::Codable,
                         public crimild::Messenger {
        public:
            enum class State {
                SUCCESS,
                FAILURE,
                RUNNING
            };

        protected:
            Behavior( void );

        public:
            virtual ~Behavior( void );

            /**
               \brief Invoked the first time a behavior is invoked by its parent

               It may be invoked multiple times if the parent behavior is
               reset or restarted
            */
            virtual void init( BehaviorContext *context );

            /**
               \brief Invoked every update of the behavior tree
            */
            virtual State step( BehaviorContext *context ) = 0;

            /**
               \name Coding support
            */
            //@{

        public:
            virtual void encode( coding::Encoder &encoder ) override;
            virtual void decode( coding::Decoder &decoder ) override;

            //@}
        };

        using BehaviorPtr = crimild::SharedPointer< Behavior >;
    }

}

#endif
