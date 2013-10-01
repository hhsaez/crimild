#include "Technique.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Technique::Technique( void )
{

}

collada::Technique::~Technique( void )
{

}

bool collada::Technique::parseXML( xmlNode *input )
{
	XMLUtils::getAttribute( input, COLLADA_SID, _sid );

	return XMLUtils::parseChild( input, COLLADA_PHONG, _phong );
}

