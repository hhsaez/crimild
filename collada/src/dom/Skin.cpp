#include "Skin.hpp"

using namespace crimild;
using namespace crimild::collada;

Skin::Skin( void )
{

}

Skin::~Skin( void )
{

}

bool Skin::parseXML( xmlNode *input )
{
	XMLUtils::getAttribute( input, COLLADA_SOURCE, _sourceID );

	_sources.parseXML( input );

	XMLUtils::parseChild( input, COLLADA_JOINTS, _joints );
	XMLUtils::parseChild( input, COLLADA_VERTEX_WEIGHTS, _vertexWeights );

	return true;
}

