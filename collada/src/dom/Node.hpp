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

			inline const char *getName( void ) const { return _name.c_str(); }
			inline const char *getSID( void ) const { return _sid.c_str(); }
			inline const char *getType( void ) const { return _type.c_str(); }

			inline collada::Matrix *getMatrix( void ) const { return _matrix.get(); }
			inline collada::InstanceController *getInstanceController( void ) { return _instanceController.get(); }
			inline collada::NodeList *getNodes( void ) const { return _nodes; }

		private:
			std::string _name;
			std::string _sid;
			std::string _type;

			collada::MatrixPtr _matrix;
			collada::InstanceControllerPtr _instanceController;
			NodeList *_nodes;
		};

		typedef std::shared_ptr< Node > NodePtr;

		class NodeList : public EntityList< collada::Node > {
		public:
			NodeList( void ) : EntityList< collada::Node >( COLLADA_NODE ) { }
			virtual ~NodeList( void ) { }
		};

	}

}

#endif

