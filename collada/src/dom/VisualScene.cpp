#include "VisualScene.hpp"

using namespace crimild;
using namespace crimild::collada;

VisualScene::VisualScene( void )
{

}

VisualScene::~VisualScene( void )
{

}

bool VisualScene::parseXML( xmlNode *input )
{
	if ( !Entity::parseXML( input ) ) {
		return false;
	}

	return _nodes.parseXML( input );
}

