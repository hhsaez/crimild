#include "VertexWeights.hpp"

using namespace crimild;
using namespace crimild::collada;

VertexWeights::VertexWeights( void )
{

}

VertexWeights::~VertexWeights( void )
{

}

bool VertexWeights::parseXML( xmlNode *input )
{
	int count = 0;
	XMLUtils::getAttribute( input, COLLADA_COUNT, count );

	if ( count > 0 ) {
		xmlNode *indexArray = XMLUtils::getChildXMLNodeWithName( input, COLLADA_V );
		if ( indexArray ) {
			xmlChar *content = xmlNodeGetContent( indexArray );
			if ( content ) {
				std::stringstream str;
				str << content;
				_indices.resize( 2 * count );
				int *data = &_indices[ 0 ];
				for ( unsigned int i = 0; i < 2 * count; i++ ) {
					str >> data[ i ];
				}
				xmlFree( content );
			}
			else {
				Log::Error << "Cannot obtain content for vertex indices" << Log::End;
			}
		}
		else {
			Log::Error << "Cannot obtain child element 'v' in vertex_weights" << Log::End;
			return false;
		}
	}
	else {
		Log::Warning << "Not enough data found in vertex_weight element" << Log::End;
	}

	return true;
}

