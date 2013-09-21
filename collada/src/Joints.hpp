#ifndef CRIMILD_PERSISTENCE_COLLADA_JOINTS_
#define CRIMILD_PERSISTENCE_COLLADA_JOINTS_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Input.hpp"

namespace crimild {

	namespace collada {

		class Joints : public Entity {
		public:
			Joints( void );

			virtual ~Joints( void );

			bool parseXML( xmlNode *input );

			inline InputLibrary *getInputLibrary( void ) { return &_inputLibrary; }

		private:
			InputLibrary _inputLibrary;
		};

		typedef std::shared_ptr< Joints > JointsPtr;

	}

}

#endif

