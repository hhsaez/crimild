#include "Triangles.hpp"

using namespace crimild;
using namespace crimild::collada;

Triangles::Triangles( void )
{
	_count = 0;
}

Triangles::~Triangles( void )
{

}

bool Triangles::parseXML( xmlNode *input )
{
	_inputs.parseXML( input );

	if ( !XMLUtils::getAttribute( input, COLLADA_COUNT, _count ) ) {
		return false;
	}

	XMLUtils::getAttribute( input, COLLADA_MATERIAL, _material, false );

	xmlNode *pArray = XMLUtils::getChildXMLNodeWithName( input, COLLADA_P );
	if ( pArray ) {
		xmlChar *pContent = xmlNodeGetContent( pArray );
		if ( pContent ) {
			_indices.resize( _count * 3 * _inputs.getCount() );
			std::stringstream str;
			str << pContent;
			for ( unsigned int i = 0; i < _indices.size(); i++ ) {
				str >> _indices[ i ];
			}
			xmlFree( pContent );
		}
		else {
			Log::Error << "Cannot obtain content for p element" << Log::End;
			return false;
		}
	}
	else {
		Log::Error << "No primitive indices provided in triangle object" << Log::End;
		return false;
	}

	return true;
}

