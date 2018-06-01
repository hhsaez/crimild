#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_AVOID_WALLS_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_AVOID_WALLS_

#include "Behaviors/Behavior.hpp"

namespace crimild {

	namespace behaviors {

		namespace actions {

			class MotionAvoidWalls : public Behavior {
				CRIMILD_IMPLEMENT_RTTI( crimild::behaviors::actions::MotionAvoidWalls )
				
			public:
				explicit MotionAvoidWalls( void );
				virtual ~MotionAvoidWalls( void );
				
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

