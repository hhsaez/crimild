#include "Entity.hpp"

using namespace crimild;
using namespace crimild::collada;

Entity::Entity( void )
{

}

Entity::~Entity( void )
{

}

bool Entity::parseXML( xmlNode *input )
{
	return XMLUtils::getAttribute( input, COLLADA_ID, _id );
}

