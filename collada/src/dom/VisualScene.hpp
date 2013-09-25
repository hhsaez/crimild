#ifndef CRIMILD_COLLADA_VISUAL_SCENE_
#define CRIMILD_COLLADA_VISUAL_SCENE_

#include "Entity.hpp"
#include "EntityList.hpp"
#include "Input.hpp"
#include "Node.hpp"

namespace crimild {

	namespace collada {

		class VisualScene : public Entity {
		public:
			VisualScene( void );

			virtual ~VisualScene( void );

			bool parseXML( xmlNode *input );

			inline collada::NodeList *getNodes( void ) { return &_nodes; }

		private:
			collada::NodeList _nodes;
		};

		typedef std::shared_ptr< VisualScene > VisualScenePtr;

		class VisualSceneList : public EntityList< VisualScene > {
		public:
			VisualSceneList( void ) : EntityList< VisualScene >( COLLADA_VISUAL_SCENE ) { }
			virtual ~VisualSceneList( void ) { }
		};

	}

}

#endif

