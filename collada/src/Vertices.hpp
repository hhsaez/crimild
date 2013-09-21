#ifndef CRIMILD_COLLADA_VERTICES_
#define CRIMILD_COLLADA_VERTICES_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Input.hpp"

namespace crimild {

	namespace collada {

		class Vertices : public Entity {
		public:
			Vertices( void );

			virtual ~Vertices( void );

			bool parseXML( xmlNode *input );

			inline InputLibrary *getInputLibrary( void ) { return &_inputLibrary; }

		private:
			InputLibrary _inputLibrary;
		};

		typedef std::shared_ptr< Vertices > VerticesPtr;

	}

}

#endif

