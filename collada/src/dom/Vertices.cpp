#include "Vertices.hpp"

using namespace crimild;
using namespace crimild::collada;

Vertices::Vertices( void )
{

}

Vertices::~Vertices( void )
{

}

bool Vertices::parseXML( xmlNode *input )
{
	if ( !Entity::parseXML( input ) ) {
		return false;
	}

	return _inputs.parseXML( input );
}

