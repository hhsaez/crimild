#include "Phong.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Phong::Phong( void )
{

}

collada::Phong::~Phong( void )
{

}

bool collada::Phong::parseXML( xmlNode *input )
{
	XMLUtils::parseChild( input, COLLADA_DIFFUSE, _diffuse );
	
	return true;
}

