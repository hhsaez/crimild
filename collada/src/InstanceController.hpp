#ifndef CRIMILD_COLLADA_INSTANCE_CONTROLLER_
#define CRIMILD_COLLADA_INSTANCE_CONTROLLER_

#include "Entity.hpp"

namespace crimild {

	namespace collada {

		class InstanceController : public Entity {
		public:
			InstanceController( void );
			virtual ~InstanceController( void );

			bool parseXML( xmlNode *input );

			inline const char *getControllerID( void ) const { return _controllerID.c_str(); }

		private:
			std::string _controllerID;
		};

		typedef std::shared_ptr< InstanceController > InstanceControllerPtr;

	}

}

#endif

