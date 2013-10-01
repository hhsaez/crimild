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

			virtual bool parseXML( xmlNode *input ) override;

			InputList *getInputs( void ) { return &_inputs; }
			unsigned int getCount( void ) const { return _count; }
			const unsigned int *getIndices( void ) const { return &_indices[ 0 ]; }
			std::string getMaterial( void ) const { return _material; }

		private:
			unsigned int _count;
			InputList _inputs;
			std::vector< unsigned int > _indices;
			std::string _material;
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

