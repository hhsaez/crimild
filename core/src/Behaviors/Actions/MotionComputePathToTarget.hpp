#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_COMPUTE_PATH_TO_TARGET_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_COMPUTE_PATH_TO_TARGET_

#include "Behaviors/Behavior.hpp"

namespace crimild {

	namespace behaviors {

		namespace actions {

			class MotionComputePathToTarget : public Behavior {
				CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::actions::MotionComputePathToTarget )
				
			public:
				explicit MotionComputePathToTarget( void );
				virtual ~MotionComputePathToTarget( void );
				
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

