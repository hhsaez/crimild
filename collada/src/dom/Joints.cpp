#include "Joints.hpp"

using namespace crimild;
using namespace crimild::collada;

Joints::Joints( void )
{

}

Joints::~Joints( void )
{

}

bool Joints::parseXML( xmlNode *input )
{
	return _inputs.parseXML( input );
}

