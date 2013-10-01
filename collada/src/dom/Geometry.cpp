#include "Geometry.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Geometry::Geometry( void )
{

}

collada::Geometry::~Geometry( void )
{

}

bool collada::Geometry::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <geometry> object" << Log::End;

	if ( !Entity::parseXML( input ) ) {
		Log::Debug << "No id attribute provided for geometry object" << Log::End;
		return false;
	}

	XMLUtils::getAttribute( input, COLLADA_NAME, _name );
	return XMLUtils::parseChild( input, COLLADA_MESH, _mesh );
}

