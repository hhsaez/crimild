#include "Accessor.hpp"

using namespace crimild;
using namespace crimild::collada;

Accessor::Accessor( void )
{
}

Accessor::~Accessor( void )
{
}

bool Accessor::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <accessor> object" << Log::End;

	XMLUtils::getAttribute( input, COLLADA_COUNT, _count );
	XMLUtils::getAttribute( input, COLLADA_STRIDE, _stride );

	_params.parseXML( input );

	return true;
}

