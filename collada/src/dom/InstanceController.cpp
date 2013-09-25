#include "InstanceController.hpp"

using namespace crimild;
using namespace crimild::collada;

InstanceController::InstanceController( void )
{

}

InstanceController::~InstanceController( void )
{

}

bool InstanceController::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <instance_controller> element" << Log::End;

	xmlChar *urlProp = xmlGetProp( input, ( xmlChar * ) COLLADA_URL );
	if ( urlProp == NULL ) {
		Log::Error << "No url attribute found in instance controller element" << Log::End;
		return false;
	}
	
	_controllerID = std::string( ( const char * ) urlProp ).substr( 1 );
	xmlFree( urlProp );

	return true;
}

