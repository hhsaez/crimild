#include "Effect.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Effect::Effect( void )
{

}

collada::Effect::~Effect( void )
{

}

bool collada::Effect::parseXML( xmlNode *input )
{
	if ( !Entity::parseXML( input ) ) {
		return false;
	}

	XMLUtils::getAttribute( input, COLLADA_NAME, _name );

	return XMLUtils::parseChild( input, COLLADA_PROFILE_COMMON, _profileCommon );
}

