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
	Log::Debug << "Parsing <visual_scene> object" << Log::End;

	xmlChar *idProp = xmlGetProp( input, ( xmlChar * ) COLLADA_ID );
	if ( !idProp ) {
		Log::Error << "No id attribute provided for visual scene object" << Log::End;
		return false;
	}

	setID( ( const char * ) idProp );
	xmlFree( idProp );

	_nodes.parseXML( input );

	return true;
}

