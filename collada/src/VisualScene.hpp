#ifndef CRIMILD_COLLADA_VISUAL_SCENE_
#define CRIMILD_COLLADA_VISUAL_SCENE_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Input.hpp"
#include "Node.hpp"

namespace crimild {

	namespace collada {

		class VisualScene : public Entity {
		public:
			VisualScene( void );

			virtual ~VisualScene( void );

			bool parseXML( xmlNode *input );

			inline collada::NodeLibrary *getNodeLibrary( void ) { return &_nodeLibrary; }

		private:
			collada::NodeLibrary _nodeLibrary;
		};

		typedef std::shared_ptr< VisualScene > VisualScenePtr;

		class VisualSceneLibrary : public EntityLibrary< VisualScene > {
		public:
			VisualSceneLibrary( void ) : EntityLibrary< VisualScene >( COLLADA_VISUAL_SCENE ) { }
			virtual ~VisualSceneLibrary( void ) { }
		};

	}

}

#endif

