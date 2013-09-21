#ifndef CRIMILD_COLLADA_GEOMETRY_
#define CRIMILD_COLLADA_GEOMETRY_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Mesh.hpp"

namespace crimild {

	namespace collada {

		class Geometry : public Entity {
		public:
			Geometry( void );

			virtual ~Geometry( void );

			bool parseXML( xmlNode *input );

			inline const char *getName( void ) const { return _name.c_str(); }
			inline Mesh *getMesh( void ) { return _mesh.get(); }

		private:
			std::string _name;
			MeshPtr _mesh;
		};

		typedef std::shared_ptr< Geometry > GeometryPtr;

		class GeometryLibrary : public EntityLibrary< Geometry > {
		public:
			GeometryLibrary( void ) : EntityLibrary< Geometry >( COLLADA_GEOMETRY ) { }
			virtual ~GeometryLibrary( void ) { }
		};

	}

}

#endif

