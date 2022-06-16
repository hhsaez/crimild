#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_RESET_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_RESET_

#include "Behaviors/Behavior.hpp"

namespace crimild {

    namespace behaviors {

        namespace actions {

            /**
             * \brief Resets motion state and create default values if needed.
             *
             * During initialization, this behavior will fetch the current motion
             * state (position, velocity and steering) from the context or define
             * default values for each of the variables. Therefore, this behavior
             * is usually the first one in the a sequence of motion behaviors.
             *
             * During each step, the agent position is copied to the corresponding
             * field in motion state. Steering is reset too. Velocity is **not**
             * changed, though.
             */
            class MotionReset : public Behavior {
                CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::actions::MotionReset )

            public:
                virtual void init( crimild::behaviors::BehaviorContext *context ) override;
                virtual crimild::behaviors::Behavior::State step( crimild::behaviors::BehaviorContext *context ) override;

            private:
                SharedPointer< Variant > m_position;
                SharedPointer< Variant > m_velocity;
                SharedPointer< Variant > m_steering;

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
