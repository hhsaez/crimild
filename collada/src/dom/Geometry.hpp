#ifndef CRIMILD_COLLADA_GEOMETRY_
#define CRIMILD_COLLADA_GEOMETRY_

#include "Entity.hpp"
#include "EntityList.hpp"
#include "Mesh.hpp"

namespace crimild {

	namespace collada {

		class Geometry : public Entity {
		public:
			Geometry( void );

			virtual ~Geometry( void );

			virtual bool parseXML( xmlNode *input ) override;

			inline const char *getName( void ) const { return _name.c_str(); }
			inline Mesh *getMesh( void ) { return _mesh.get(); }

		private:
			std::string _name;
			Pointer< Mesh > _mesh;
		};

		class GeometryList : public EntityList< Geometry > {
		public:
			GeometryList( void ) : EntityList< Geometry >( COLLADA_GEOMETRY ) { }
			virtual ~GeometryList( void ) { }
		};

	}

}

#endif

