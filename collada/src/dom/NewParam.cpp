#include "NewParam.hpp"

using namespace crimild;
using namespace crimild::collada;

NewParam::NewParam( void )
{
}

NewParam::~NewParam( void )
{

}

bool NewParam::parseXML( xmlNode *input )
{
	XMLUtils::getAttribute( input, COLLADA_SID, _sid );
	setID( _sid );

	XMLUtils::parseChild( input, COLLADA_SAMPLER_2D, _sampler2D );
	XMLUtils::parseChild( input, COLLADA_SURFACE, _surface );

	return true;
}

