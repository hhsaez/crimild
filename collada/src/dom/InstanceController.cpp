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
	return XMLUtils::getAttribute( input, COLLADA_URL, _controllerID );
}

