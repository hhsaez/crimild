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
	Log::Debug << "Parsing <technique_common> element" << Log::End;
	return XMLUtils::parseChild( input, COLLADA_ACCESSOR, _accessor );
}

