#ifndef CRIMILD_EXAMPLES_BRICKS_COMPONENTS_GAME_CONTROLLER_
#define CRIMILD_EXAMPLES_BRICKS_COMPONENTS_GAME_CONTROLLER_

#include <Crimild.hpp>
#include <Crimild_Scripting.hpp>

namespace crimild {

	namespace bricks {

		class GameController : 
			public NodeComponent,
			public Messenger {
		public:
			GameController( scripting::ScriptEvaluator &eval );
			virtual ~GameController( void );

			virtual void start( void ) override;
			virtual void update( const Clock & ) override;
		};

	}

}

#endif

