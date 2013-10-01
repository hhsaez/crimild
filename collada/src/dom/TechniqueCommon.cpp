#include "TechniqueCommon.hpp"

using namespace crimild;
using namespace crimild::collada;

TechniqueCommon::TechniqueCommon( void )
{

}

TechniqueCommon::~TechniqueCommon( void )
{

}

bool TechniqueCommon::parseXML( xmlNode *input )
{
	return XMLUtils::parseChild( input, COLLADA_ACCESSOR, _accessor );
}

