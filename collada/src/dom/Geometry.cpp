#include "Geometry.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Geometry::Geometry( void )
{

}

collada::Geometry::~Geometry( void )
{

}

bool collada::Geometry::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <geometry> object" << Log::End;

	xmlChar *idProp = xmlGetProp( input, ( xmlChar * ) COLLADA_ID );
	if ( !idProp ) {
		Log::Debug << "No id attribute provided for geometry object" << Log::End;
		return false;
	}

	setID( ( const char * ) idProp );
	xmlFree( idProp );

	xmlChar *nameProp = xmlGetProp( input, ( xmlChar * ) COLLADA_NAME );
	if ( nameProp != nullptr ) {
		_name = ( const char * ) nameProp;
		xmlFree( nameProp );
	}
	else {
		Log::Warning << "No name attribute provided for geometry object" << Log::End;
	}

	xmlNode *meshXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_MESH );
	if ( !meshXML ) {
		Log::Error << "No mesh information provided for geometry " << getID() << Log::End;
		return false;
	}

	MeshPtr mesh( new Mesh() );
	if ( !mesh->parseXML( meshXML ) ) {
		Log::Error << "Error while parsing mesh object" << Log::End;
		return false;
	}
	
	_mesh = mesh;

	return true;
}

