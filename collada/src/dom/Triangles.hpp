#ifndef CRIMILD_COLLADA_TRIANGLES_
#define CRIMILD_COLLADA_TRIANGLES_

#include "Entity.hpp"
#include "EntityList.hpp"
#include "Input.hpp"

namespace crimild {

	namespace collada {

		class Triangles : public Entity {
		public:
			Triangles( void );

			virtual ~Triangles( void );

			bool parseXML( xmlNode *input );

			inline InputList *getInputs( void ) { return &_inputs; }
			inline unsigned int getCount( void ) const { return _count; }
			inline const unsigned int *getIndices( void ) const { return &_indices[ 0 ]; }

		private:
			unsigned int _count;
			InputList _inputs;
			std::vector< unsigned int > _indices;
		};

		typedef std::shared_ptr< Triangles > TrianglesPtr;

		class TrianglesList : public EntityList< Triangles > {
		public:
			TrianglesList( void ) : EntityList< Triangles >( COLLADA_TRIANGLES ) { }
			virtual ~TrianglesList( void ) { }
		};

	}

}

#endif

