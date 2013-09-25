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
	Log::Debug << "Parsing sampler" << Log::End;

	xmlChar *idProp = xmlGetProp( input, ( xmlChar * ) COLLADA_ID );
	if ( !idProp ) {
		Log::Debug << "No id attribute provided for sampler object" << Log::End;
		return false;
	}

	setID( ( const char * ) idProp );
	xmlFree( idProp );

	_inputs.parseXML( input );

	return true;
}

