#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_APPLY_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_APPLY_

#include "Behaviors/Behavior.hpp"

namespace crimild {

    class MotionState;

    namespace behaviors {

        namespace actions {

            /**
             * \brief Transforms agent based on current motion state
             *
             * This is usually the last one in a sequence of motion behaviors. It should be
             * used alognside MotionReset
             */
            class MotionApply : public Behavior {
                CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::actions::MotionApply )

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
