#ifndef CRIMILD_COLLADA_MESH_
#define CRIMILD_COLLADA_MESH_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Source.hpp"
#include "Vertices.hpp"
#include "Triangles.hpp"

namespace crimild {

	namespace collada {

		class Mesh : public Entity {
		public:
			Mesh( void );

			virtual ~Mesh( void );

			bool parseXML( xmlNode *input );

			inline SourceCatalog *getSourceCatalog( void ) { return &_sources; }
			inline Vertices *getVertices( void ) { return _vertices.get(); }
			inline Triangles *getTriangles( void ) { return _triangles.get(); }

		private:
			SourceCatalog _sources;
			VerticesPtr _vertices;
			TrianglesPtr _triangles;
		};

		typedef std::shared_ptr< Mesh > MeshPtr;

	}

}

#endif

