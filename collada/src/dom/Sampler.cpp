#include "Sampler.hpp"

using namespace crimild;
using namespace crimild::collada;

Sampler::Sampler( void )
{

}

Sampler::~Sampler( void )
{

}

bool Sampler::parseXML( xmlNode *input )
{
	if ( !Entity::parseXML( input ) ) {
		return false;
	}

	return _inputs.parseXML( input );
}

