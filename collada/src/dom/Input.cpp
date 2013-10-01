#include "Input.hpp"

using namespace crimild;
using namespace crimild::collada;

Input::Input( void )
{
	_source = NULL;
}

Input::~Input( void )
{

}

bool Input::parseXML( xmlNode *input )
{
	if ( !XMLUtils::getAttribute( input, COLLADA_SEMANTIC, _semantic ) ) {
		return false;
	}

	if ( !XMLUtils::getAttribute( input, COLLADA_SOURCE, _sourceID ) ) {
		return false;
	}

	return true;
}

