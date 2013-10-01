#include "Surface.hpp"

using namespace crimild;
using namespace crimild::collada;

Surface::Surface( void )
{
}

Surface::~Surface( void )
{

}

bool Surface::parseXML( xmlNode *input )
{
	XMLUtils::getAttribute( input, COLLADA_TYPE, _type );

	xmlNode *initFrom = XMLUtils::getChildXMLNodeWithName( input, COLLADA_INIT_FROM );
	if ( initFrom != nullptr ) {
		xmlChar *content = xmlNodeGetContent( initFrom );
		if ( content != nullptr ) {
			_imageID = ( const char * ) content;
		}
	}

	return true;
}

