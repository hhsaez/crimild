#ifndef CRIMILD_COLLADA_NODE_
#define CRIMILD_COLLADA_NODE_

#include "Entity.hpp"
#include "EntityList.hpp"
#include "Matrix.hpp"
#include "InstanceController.hpp"

namespace crimild {

	namespace collada {

		class NodeList;

		class Node : public Entity {
		public:
			Node( void );

			virtual ~Node( void );

			virtual bool parseXML( xmlNode *input ) override;

			const char *getName( void ) const { return _name.c_str(); }
			const char *getSID( void ) const { return _sid.c_str(); }
			const char *getType( void ) const { return _type.c_str(); }

			const collada::Matrix *getMatrix( void ) const { return _matrix.get(); }
			collada::Matrix *getMatrix( void ) { return _matrix.get(); }

			const collada::InstanceController *getInstanceController( void ) const { return _instanceController.get(); }
			collada::InstanceController *getInstanceController( void ) { return _instanceController.get(); }

			const collada::NodeList *getNodes( void ) const { return _nodes; }
			collada::NodeList *getNodes( void ) { return _nodes; }

		private:
			std::string _name;
			std::string _sid;
			std::string _type;

			Pointer< collada::Matrix > _matrix;
			Pointer< collada::InstanceController > _instanceController;
			NodeList *_nodes;
		};

		class NodeList : public EntityList< collada::Node > {
		public:
			NodeList( void ) : EntityList< collada::Node >( COLLADA_NODE ) { }
			virtual ~NodeList( void ) { }
		};

	}

}

#endif

