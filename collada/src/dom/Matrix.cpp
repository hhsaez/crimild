#include "Matrix.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Matrix::Matrix( void )
{

}

collada::Matrix::~Matrix( void )
{

}

bool collada::Matrix::parseXML( xmlNode *input )
{
	xmlChar *content = xmlNodeGetContent( input );
	if ( content != NULL ) {
		std::stringstream str;
		str << content;
		for ( unsigned int i = 0; i < 16; i++ ) {
			str >> _data[ i ];
		}
		xmlFree( content );
	}

	return true;
}

