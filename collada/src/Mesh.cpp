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

	xmlNode *trianglesXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_TRIANGLES );
	if ( trianglesXML ) {
		TrianglesPtr triangles( new Triangles() );
		if ( triangles->parseXML( trianglesXML ) ) {
			_triangles = triangles;
		}
	}
	else {
		Log::Error << "No triangle information provided for mesh" << Log::End;
		return false;
	}

	return true;
}

