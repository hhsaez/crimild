#include "Controller.hpp"

using namespace crimild;
using namespace crimild::collada;

Controller::Controller( void )
{

}

Controller::~Controller( void )
{

}

bool Controller::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <controller> object" << Log::End;

	xmlChar *idProp = xmlGetProp( input, ( xmlChar * ) COLLADA_ID );
	if ( !idProp ) {
		Log::Error << "No id attribute provided for controller object" << Log::End;
		return false;
	}

	setID( ( const char * ) idProp );
	xmlFree( idProp );

	xmlNode *skinXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_SKIN );
	if ( skinXML ) {
		SkinPtr skin( new Skin() );
		if ( skin->parseXML( skinXML ) ) {
			_skin = skin;
		}
	}

	return true;
}

