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

	XMLUtils::parseChild( input, COLLADA_VERTICES, _vertices );

	_triangles.parseXML( input );

	return true;
}

