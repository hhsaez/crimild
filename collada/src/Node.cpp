#include "Node.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Node::Node( void )
{
	_nodeLibrary = new NodeLibrary();
}

collada::Node::~Node( void )
{
	delete _nodeLibrary;
}

bool collada::Node::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <node> object" << Log::End;

	xmlChar *idProp = xmlGetProp( input, ( xmlChar * ) COLLADA_ID );
	if ( idProp != NULL ) {
		setID( ( const char * ) idProp );
		xmlFree( idProp );
	}
	else {
		Log::Warning << "No id attribute provided for node object" << Log::End;
	}

	xmlChar *nameProp = xmlGetProp( input, ( xmlChar * ) COLLADA_NAME );
	if ( nameProp != NULL ) {
		_name = ( const char * ) nameProp;
		xmlFree( nameProp );
	}
	else {
		Log::Warning << "No id attribute provided for sampler object" << Log::End;
	}

	xmlChar *sidProp = xmlGetProp( input, ( xmlChar * ) COLLADA_SID );
	if ( sidProp != NULL ) {
		_sid = ( const char * ) sidProp;
		xmlFree( sidProp );
	}

	xmlChar *typeProp = xmlGetProp( input, ( xmlChar * ) COLLADA_TYPE );
	if ( typeProp != NULL ) {
		_type = ( const char * ) typeProp;
		xmlFree( typeProp );
	}

	xmlNode *matrixXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_MATRIX );
	if ( matrixXML ) {
		collada::MatrixPtr matrix( new collada::Matrix() );
		if ( matrix->parseXML( matrixXML ) ) {
			_matrix = matrix;
		}
	}

	xmlNode *instanceControllerXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_INSTANCE_CONTROLLER );
	if ( instanceControllerXML ) {
		InstanceControllerPtr instanceController( new InstanceController() );
		if ( instanceController->parseXML( instanceControllerXML ) ) {
			_instanceController = instanceController;
		}
	}

	_nodeLibrary->parseXML( input );

	return true;
}

