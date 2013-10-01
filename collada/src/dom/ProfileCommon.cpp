#include "ProfileCommon.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::ProfileCommon::ProfileCommon( void )
{

}

collada::ProfileCommon::~ProfileCommon( void )
{

}

bool collada::ProfileCommon::parseXML( xmlNode *input )
{
	_newparams.parseXML( input );
	
	return XMLUtils::parseChild( input, COLLADA_TECHNIQUE, _technique );
}

