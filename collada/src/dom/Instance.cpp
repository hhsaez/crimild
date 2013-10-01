#include "Instance.hpp"

using namespace crimild;
using namespace crimild::collada;

Instance::Instance( void )
{

}

Instance::~Instance( void )
{

}

bool Instance::parseXML( xmlNode *input )
{
	Entity::parseXML( input );
	XMLUtils::getAttribute( input, COLLADA_URL, _url );
	return ( getID() != nullptr || getUrl() != nullptr );
}

