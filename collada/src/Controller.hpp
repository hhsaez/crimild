#ifndef CRIMILD_COLLADA_CONTROLLER_
#define CRIMILD_COLLADA_CONTROLLER_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Input.hpp"
#include "Skin.hpp"

namespace crimild {

	namespace collada {

		class Controller : public Entity {
		public:
			Controller( void );

			virtual ~Controller( void );

			bool parseXML( xmlNode *input );

			inline Skin *getSkin( void ) { return _skin.get(); }

		private:
			SkinPtr _skin;
		};

		typedef std::shared_ptr< Controller > ControllerPtr;

		class ControllerLibrary : public EntityLibrary< Controller > {
		public:
			ControllerLibrary( void ) : EntityLibrary< Controller >( COLLADA_CONTROLLER ) { }
			virtual ~ControllerLibrary( void ) { }
		};

	}

}

#endif

