#include "Input.hpp"

using namespace crimild;
using namespace crimild::collada;

Input::Input( void )
{
	_source = NULL;
}

Input::~Input( void )
{

}

bool Input::parseXML( xmlNode *input )
{
	xmlChar *semanticProp = xmlGetProp( input, ( xmlChar * ) COLLADA_SEMANTIC );
	if ( !semanticProp ) {
		Log::Error << "No semantic attribute provided for channel object" << Log::End;
		return false;
	}

	_semantic = ( const char * ) semanticProp;
	xmlFree( semanticProp );

	xmlChar *sourceProp = xmlGetProp( input, ( xmlChar * ) COLLADA_SOURCE );
	if ( !sourceProp ) {
		Log::Error << "No source attribute provided for channel object" << Log::End;
		return false;
	}
	
	_sourceID = std::string( ( const char * ) sourceProp ).substr( 1 );
	xmlFree( sourceProp );

	Log::Debug << "Parsing <input> object with semantic " << _semantic << " " << _sourceID << Log::End;

	return true;
}

