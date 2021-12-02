#ifndef CRIMILD_EXAMPLES_BRICKS_COMPONENTS_BRICK_
#define CRIMILD_EXAMPLES_BRICKS_COMPONENTS_BRICK_

#include <Crimild.hpp>
#include <Crimild_Scripting.hpp>

namespace crimild {

	namespace bricks {

		class Brick : 
			public NodeComponent,
			public Messenger {
		public:
			Brick( scripting::ScriptEvaluator &eval );
			virtual ~Brick( void );

			virtual void start( void ) override;
			virtual void update( const Clock & ) override;
		};

	}

}

#endif

