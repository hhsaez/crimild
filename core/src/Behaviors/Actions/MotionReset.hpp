#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_RESET_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_RESET_

#include "Behaviors/Behavior.hpp"

namespace crimild {

	namespace behaviors {

		namespace actions {

			class MotionReset : public Behavior {
				CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::actions::MotionReset )
				
			public:
				explicit MotionReset( void );
				virtual ~MotionReset( void );

				virtual void init( crimild::behaviors::BehaviorContext *context ) override;
				virtual crimild::behaviors::Behavior::State step( crimild::behaviors::BehaviorContext *context ) override;

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

