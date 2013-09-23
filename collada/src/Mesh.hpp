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

			SourceCatalog *getSourceCatalog( void ) { return &_sources; }
			Vertices *getVertices( void ) { return _vertices.get(); }
			TrianglesLibrary *getTrianglesLibrary( void ) { return &_trianglesLibrary; }

		private:
			SourceCatalog _sources;
			VerticesPtr _vertices;
			TrianglesLibrary _trianglesLibrary;
		};

		typedef std::shared_ptr< Mesh > MeshPtr;

	}

}

#endif

