#include "Triangles.hpp"

using namespace crimild;
using namespace crimild::collada;

Triangles::Triangles( void )
{

}

Triangles::~Triangles( void )
{

}

bool Triangles::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <triangles> object" << Log::End;

	_inputs.parseXML( input );

	_count = 0;
	xmlChar *triangleCountXML = xmlGetProp( input, ( const xmlChar * ) COLLADA_COUNT );
	if ( triangleCountXML ) {
		_count = xmlStringToValue< unsigned int >( triangleCountXML );
		xmlFree( triangleCountXML );
	}
	else {
		Log::Error << "No count attribute found in triangle element" << Log::End;
		return false;
	}

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

