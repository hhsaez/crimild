#ifndef CRIMILD_COLLADA_TRIANGLES_
#define CRIMILD_COLLADA_TRIANGLES_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Input.hpp"

namespace crimild {

	namespace collada {

		class Triangles : public Entity {
		public:
			Triangles( void );

			virtual ~Triangles( void );

			bool parseXML( xmlNode *input );

			inline InputLibrary *getInputLibrary( void ) { return &_inputLibrary; }
			inline unsigned int getCount( void ) const { return _count; }
			inline const unsigned int *getIndices( void ) const { return &_indices[ 0 ]; }

		private:
			unsigned int _count;
			InputLibrary _inputLibrary;
			std::vector< unsigned int > _indices;
		};

		typedef std::shared_ptr< Triangles > TrianglesPtr;

	}

}

#endif

