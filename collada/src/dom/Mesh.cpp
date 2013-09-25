#include "Mesh.hpp"

using namespace crimild;
using namespace crimild::collada;

Mesh::Mesh( void )
{

}

Mesh::~Mesh( void )
{

}

bool Mesh::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <mesh> object" << Log::End;

	_sources.parseXML( input );

	xmlNode *verticesXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_VERTICES );
	if ( verticesXML ) {
		VerticesPtr vertices( new Vertices() );
		if ( vertices->parseXML( verticesXML ) ) {
			_vertices = vertices;
		}
	}
	else {
		Log::Error << "No vertices information provided for mesh" << Log::End;
		return false;
	}

	_triangles.parseXML( input );

	return true;
}

