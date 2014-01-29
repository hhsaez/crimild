#ifndef CRIMILD_COLLADA_SURFACE_
#define CRIMILD_COLLADA_SURFACE_

#include "Entity.hpp"
#include "EntityList.hpp"

namespace crimild {

	namespace collada {

		class Surface : public Entity {
		public:
			Surface( void );

			virtual ~Surface( void );

			virtual bool parseXML( xmlNode *input ) override;

			std::string getType( void ) const { return _type; }
			std::string getImageID( void ) const { return _imageID; }

		private:
			std::string _type;
			std::string _imageID;
		};

	}

}

#endif

