#ifndef CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_SEEK_
#define CRIMILD_CORE_BEHAVIORS_ACTIONS_MOTION_SEEK_

#include "Behaviors/Behavior.hpp"

namespace crimild {

	namespace behaviors {

		namespace actions {

			class MotionSeek : public Behavior {
				CRIMILD_IMPLEMENT_RTTI( huntsman::behaviors::actions::MotionSeek )
				
			public:
				explicit MotionSeek( void );
				virtual ~MotionSeek( void );
				
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

