#ifndef CRIMILD_COLLADA_PHONG_
#define CRIMILD_COLLADA_PHONG_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "Diffuse.hpp"

namespace crimild {

	namespace collada {

		class Phong : public Entity {
		public:
			Phong( void );
			virtual ~Phong( void );

			virtual bool parseXML( xmlNode *input ) override;

			Diffuse *getDiffuse( void ) { return _diffuse.get(); }

		private:
			DiffusePtr _diffuse;
		};

		typedef std::shared_ptr< collada::Phong > PhongPtr;

	}

}

#endif

