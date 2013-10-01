#include "Diffuse.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Diffuse::Diffuse( void )
{

}

collada::Diffuse::~Diffuse( void )
{

}

bool collada::Diffuse::parseXML( xmlNode *input )
{
	XMLUtils::parseChild( input, COLLADA_TEXTURE, _texture );
	return true;
}

