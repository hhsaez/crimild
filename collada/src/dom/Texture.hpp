#ifndef CRIMILD_COLLADA_TEXTURE_
#define CRIMILD_COLLADA_TEXTURE_

#include "Entity.hpp"
#include "EntityMap.hpp"

namespace crimild {

	namespace collada {

		class Texture : public Entity {
		public:
			Texture( void );
			virtual ~Texture( void );

			virtual bool parseXML( xmlNode *input ) override;

			std::string getTexture( void ) const { return _texture; }
			std::string getTexcoord( void ) const { return _texcoord; }

		private:
			std::string _texture;
			std::string _texcoord;
		};

	}

}

#endif

