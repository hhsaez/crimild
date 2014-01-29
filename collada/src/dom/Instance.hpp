#ifndef CRIMILD_COLLADA_INSTANCE_
#define CRIMILD_COLLADA_INSTANCE_

#include "Entity.hpp"

namespace crimild {

	namespace collada {

		class Instance : public Entity {
		public:
			Instance( void );
			virtual ~Instance( void );

			virtual bool parseXML( xmlNode *input ) override;

			inline const char *getUrl( void ) const { return _url.c_str(); }

		private:
			std::string _url;
		};

	}

}

#endif

