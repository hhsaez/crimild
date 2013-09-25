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
	Log::Debug << "Parsing <joints> object" << Log::End;

	_inputs.parseXML( input );

	return true;
}

