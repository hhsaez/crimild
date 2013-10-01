#ifndef CRIMILD_COLLADA_MESH_
#define CRIMILD_COLLADA_MESH_

#include "Source.hpp"
#include "Vertices.hpp"
#include "Triangles.hpp"

namespace crimild {

	namespace collada {

		class Mesh : public Entity {
		public:
			Mesh( void );

			virtual ~Mesh( void );

			virtual bool parseXML( xmlNode *input ) override;

			SourceMap *getSources( void ) { return &_sources; }
			Vertices *getVertices( void ) { return _vertices.get(); }
			TrianglesList *getTriangles( void ) { return &_triangles; }

		private:
			SourceMap _sources;
			VerticesPtr _vertices;
			TrianglesList _triangles;
		};

		typedef std::shared_ptr< Mesh > MeshPtr;

	}

}

#endif

