#ifndef CRIMILD_PERSISTENCE_COLLADA_JOINTS_
#define CRIMILD_PERSISTENCE_COLLADA_JOINTS_

#include "Input.hpp"

namespace crimild {

	namespace collada {

		class Joints : public Entity {
		public:
			Joints( void );

			virtual ~Joints( void );

			bool parseXML( xmlNode *input );

			inline InputList *getInputs( void ) { return &_inputs; }

		private:
			InputList _inputs;
		};

		typedef std::shared_ptr< Joints > JointsPtr;

	}

}

#endif

