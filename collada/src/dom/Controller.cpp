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

	if ( !Entity::parseXML( input ) ) {
		Log::Error << "No id attribute provided for controller object" << Log::End;
		return false;
	}

	xmlNode *skinXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_SKIN );
	if ( skinXML ) {
		Pointer< Skin > skin( new Skin() );
		if ( skin->parseXML( skinXML ) ) {
			_skin = skin;
		}
	}

	return true;
}

