#ifndef CRIMILD_COLLADA_NODE_
#define CRIMILD_COLLADA_NODE_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Matrix.hpp"
#include "InstanceController.hpp"

namespace crimild {

	namespace collada {

		class NodeLibrary;

		class Node : public Entity {
		public:
			Node( void );

			virtual ~Node( void );

			bool parseXML( xmlNode *input );

			inline const char *getName( void ) const { return _name.c_str(); }
			inline const char *getSID( void ) const { return _sid.c_str(); }
			inline const char *getType( void ) const { return _type.c_str(); }

			inline collada::Matrix *getMatrix( void ) const { return _matrix.get(); }
			inline collada::InstanceController *getInstanceController( void ) { return _instanceController.get(); }
			inline collada::NodeLibrary *getNodeLibrary( void ) const { return _nodeLibrary; }

		private:
			std::string _name;
			std::string _sid;
			std::string _type;

			collada::MatrixPtr _matrix;
			collada::InstanceControllerPtr _instanceController;

			NodeLibrary *_nodeLibrary;
		};

		typedef std::shared_ptr< Node > NodePtr;

		class NodeLibrary : public EntityLibrary< collada::Node > {
		public:
			NodeLibrary( void ) : EntityLibrary< collada::Node >( COLLADA_NODE ) { }
			virtual ~NodeLibrary( void ) { }
		};

	}

}

#endif

