#ifndef CRIMILD_COLLADA_VERTICES_
#define CRIMILD_COLLADA_VERTICES_

#include "Entity.hpp"
#include "Input.hpp"

namespace crimild {

	namespace collada {

		class Vertices : public Entity {
		public:
			Vertices( void );

			virtual ~Vertices( void );

			virtual bool parseXML( xmlNode *input ) override;

			inline InputList *getInputs( void ) { return &_inputs; }

		private:
			InputList _inputs;
		};

		typedef std::shared_ptr< Vertices > VerticesPtr;

	}

}

#endif

