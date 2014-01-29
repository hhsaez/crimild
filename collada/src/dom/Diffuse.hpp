#ifndef CRIMILD_COLLADA_DIFFUSE_
#define CRIMILD_COLLADA_DIFFUSE_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "Texture.hpp"

namespace crimild {

	namespace collada {

		class Diffuse : public Entity {
		public:
			Diffuse( void );
			virtual ~Diffuse( void );

			virtual bool parseXML( xmlNode *input ) override;

			collada::Texture *getTexture( void ) { return _texture.get(); }

		private:
			Pointer< collada::Texture > _texture;
		};

	}

}

#endif

