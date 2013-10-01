#include "Sampler2D.hpp"

using namespace crimild;
using namespace crimild::collada;

Sampler2D::Sampler2D( void )
{
}

Sampler2D::~Sampler2D( void )
{

}

bool Sampler2D::parseXML( xmlNode *input )
{
	xmlNode *sourceXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_SOURCE );
	if ( sourceXML != nullptr ) {
		xmlChar *content = xmlNodeGetContent( sourceXML );
		if ( content != nullptr ) {
			_sourceName = ( const char * ) content;
		}
	}

	return true;
}

