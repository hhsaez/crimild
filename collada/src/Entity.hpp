#ifndef CRIMILD_COLLADA_ENTITY_
#define CRIMILD_COLLADA_ENTITY_

#include "Utils.hpp"

namespace crimild {

	namespace collada {

		class Entity {
		public:
			virtual ~Entity( void ) { }

			inline void setID( std::string id ) { _id = id; }

			inline const char *getID( void ) const { return _id.c_str(); }

		protected:
			Entity( void ) { }

		private:
			std::string _id;
		};

		typedef std::shared_ptr< Entity > EntityPtr;

	}

}

#endif

