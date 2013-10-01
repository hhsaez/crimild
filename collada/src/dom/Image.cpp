#include "Image.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Image::Image( void )
{

}

collada::Image::~Image( void )
{

}

bool collada::Image::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <image> element" << Log::End;

	if ( !Entity::parseXML( input ) ) {
		return false;
	}

	XMLUtils::getAttribute( input, COLLADA_NAME, _name );

	xmlNode *initFrom = XMLUtils::getChildXMLNodeWithName( input, COLLADA_INIT_FROM );
	if ( initFrom != nullptr ) {
		xmlChar *content = xmlNodeGetContent( initFrom );
		if ( content != nullptr ) {
			_fullPath = ( const char * ) content;

			if ( _fullPath.length() > 0 ) {
				_fileName = _fullPath.substr( _fullPath.find_last_of( "/" ) + 1 );
			}
		}
	}

	return true;
}

