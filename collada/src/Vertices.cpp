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
	Log::Debug << "Parsing <vertices> object" << Log::End;

	xmlChar *idProp = xmlGetProp( input, ( xmlChar * ) COLLADA_ID );
	if ( !idProp ) {
		Log::Error << "No id attribute provided for geometry object" << Log::End;
		return false;
	}
	
	setID( ( const char * ) idProp );
	xmlFree( idProp );

	_inputLibrary.parseXML( input );

	return true;
}

