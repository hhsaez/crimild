#ifndef CRIMILD_EXAMPLE_BRICKS_MESSAGING_BRICK_DESTROYED_
#define CRIMILD_EXAMPLE_BRICKS_MESSAGING_BRICK_DESTROYED_

#include <Crimild.hpp>

namespace crimild {

	namespace bricks {

		struct BrickDestroyed {
			SharedPointer< Node > brick;
		};

	}

}

#endif

