#include "Node.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Node::Node( void )
{
	_nodes = new NodeList();
}

collada::Node::~Node( void )
{
	delete _nodes;
}

bool collada::Node::parseXML( xmlNode *input )
{
	Entity::parseXML( input );

	XMLUtils::getAttribute( input, COLLADA_NAME, _name );
	XMLUtils::getAttribute( input, COLLADA_TYPE, _type );
	XMLUtils::getAttribute( input, COLLADA_SID, _sid, false );

	XMLUtils::parseChild( input, COLLADA_MATRIX, _matrix, false );
	XMLUtils::parseChild( input, COLLADA_INSTANCE_CONTROLLER, _instanceController, false );

	return _nodes->parseXML( input );
}

