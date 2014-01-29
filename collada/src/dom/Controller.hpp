#ifndef CRIMILD_COLLADA_CONTROLLER_
#define CRIMILD_COLLADA_CONTROLLER_

#include "Entity.hpp"
#include "EntityList.hpp"
#include "Input.hpp"
#include "Skin.hpp"

namespace crimild {

	namespace collada {

		class Controller : public Entity {
		public:
			Controller( void );

			virtual ~Controller( void );

			virtual bool parseXML( xmlNode *input ) override;

			inline Skin *getSkin( void ) { return _skin.get(); }

		private:
			Pointer< Skin > _skin;
		};

		class ControllerList : public EntityList< Controller > {
		public:
			ControllerList( void ) : EntityList< Controller >( COLLADA_CONTROLLER ) { }
			virtual ~ControllerList( void ) { }
		};

	}

}

#endif

