#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_APPLY_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_APPLY_

#include "Behaviors/Behavior.hpp"

namespace crimild {

	namespace behaviors {

		namespace actions {

			class MotionApply : public Behavior {
				CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::actions::MotionApply )
				
			public:
				explicit MotionApply( void );
				virtual ~MotionApply( void );
				
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

