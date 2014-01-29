#ifndef CRIMILD_COLLADA_IMAGE_
#define CRIMILD_COLLADA_IMAGE_

#include "Entity.hpp"
#include "EntityMap.hpp"

namespace crimild {

	namespace collada {

		class Image : public Entity {
		public:
			Image( void );

			virtual ~Image( void );

			virtual bool parseXML( xmlNode *input ) override;

			std::string getName( void ) const { return _name; }
			std::string getFullPath( void ) const { return _fullPath; }
			std::string getFileName( void ) const { return _fileName; }

		private:
			std::string _name;
			std::string _fullPath;
			std::string _fileName;
		};

		class ImageMap : public EntityMap< collada::Image > {
		public:
			ImageMap( void ) : EntityMap< collada::Image >( COLLADA_IMAGE ) { }
			virtual ~ImageMap( void ) { }
		};

	}

}

#endif

