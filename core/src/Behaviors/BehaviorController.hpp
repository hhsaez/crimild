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

#ifndef CRIMILD_CORE_BEHAVIORS_BEHAVIOR_CONTROLLER_
#define CRIMILD_CORE_BEHAVIORS_BEHAVIOR_CONTROLLER_

#include "Components/NodeComponent.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Messaging/MessageQueue.hpp"

namespace crimild {

    namespace behaviors {

        class Behavior;
        class BehaviorTree;
        class BehaviorContext;

        class BehaviorController : public crimild::NodeComponent,
                                   public crimild::Messenger {
            CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::BehaviorController )

        public:
            static constexpr const crimild::Char *DEFAULT_BEHAVIOR_NAME = "__default__";
            static constexpr const crimild::Char *SCENE_STARTED_BEHAVIOR_NAME = "__scene_started__";

            static constexpr const crimild::Char *SETTINGS_EXECUTE_BEHAVIORS = "execute_behaviors";

        public:
            explicit BehaviorController( void );
            virtual ~BehaviorController( void );

            virtual void onAttach( void ) override;
            virtual void onDetach( void ) override;

            virtual void start( void ) override;
            virtual void update( const crimild::Clock & ) override;

        public:
            crimild::behaviors::BehaviorContext *getContext( void ) { return crimild::get_ptr( _context ); }

        public:
            void attachBehaviorTree( SharedPointer< behaviors::BehaviorTree > const &behaviorTree );
            void attachBehaviorTree( std::string eventName, SharedPointer< behaviors::BehaviorTree > const &behaviorTree );

            /**
			 \brief Executes the behavior tree matching the given name

			\remarks If no behavior is found, the current behavior is not modified
			*/
            bool executeBehaviorTree( std::string name );

            inline crimild::behaviors::BehaviorTree *getBehaviorTree( std::string name = DEFAULT_BEHAVIOR_NAME ) { return crimild::get_ptr( _behaviors[ name ] ); }

        private:
            crimild::behaviors::BehaviorTree *getCurrentBehaviorTree( void ) { return _currentBehaviorTree; }
            void setCurrentBehaviorTree( crimild::behaviors::BehaviorTree *behaviorTree ) { _currentBehaviorTree = behaviorTree; }

        private:
            Map< std::string, SharedPointer< crimild::behaviors::BehaviorTree > > _behaviors;
            crimild::behaviors::BehaviorTree *_currentBehaviorTree = nullptr;
            std::string _currentEvent;
            SharedPointer< crimild::behaviors::BehaviorContext > _context;

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

#endif
