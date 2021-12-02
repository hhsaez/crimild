#ifndef CRIMILD_EXAMPLES_BRICKS_COMPONENTS_BALL_
#define CRIMILD_EXAMPLES_BRICKS_COMPONENTS_BALL_

#include <Crimild.hpp>
#include <Crimild_Scripting.hpp>

namespace crimild {

	namespace bricks {

		class Ball : 
			public NodeComponent,
			public Messenger {
		public:
			Ball( scripting::ScriptEvaluator &eval );
			virtual ~Ball( void );

			virtual void start( void ) override;
			virtual void update( const Clock & ) override;
		};

	}

}

#endif

