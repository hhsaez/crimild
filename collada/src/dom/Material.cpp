#include "Material.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Material::Material( void )
{

}

collada::Material::~Material( void )
{

}

bool collada::Material::parseXML( xmlNode *input )
{
	if ( !Entity::parseXML( input ) ) {
		return false;
	}

	XMLUtils::getAttribute( input, COLLADA_NAME, _name );
	
	return XMLUtils::parseChild( input, COLLADA_INSTANCE_EFFECT, _instanceEffect );
}

