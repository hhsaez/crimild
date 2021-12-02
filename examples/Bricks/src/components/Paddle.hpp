#ifndef CRIMILD_EXAMPLES_BRICKS_COMPONENTS_PADDLE_
#define CRIMILD_EXAMPLES_BRICKS_COMPONENTS_PADDLE_

#include <Crimild.hpp>
#include <Crimild_Scripting.hpp>

namespace crimild {

	namespace bricks {

		class Paddle : 
			public NodeComponent,
			public Messenger {
		public:
			Paddle( scripting::ScriptEvaluator &eval );
			virtual ~Paddle( void );

			virtual void start( void ) override;
			virtual void update( const Clock & ) override;

		private:
			float _speed = 1.0f;
		};

	}

}

#endif

