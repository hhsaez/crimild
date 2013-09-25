#include "Channel.hpp"

using namespace crimild;
using namespace crimild::collada;

Channel::Channel( void )
{
	_sampler = NULL;
}

Channel::~Channel( void )
{

}

bool Channel::parseXML( xmlNode *node )
{
	Log::Debug << "Parsing channel" << Log::End;

	xmlChar *sourceProp = xmlGetProp( node, ( xmlChar * ) COLLADA_SOURCE );
	if ( !sourceProp ) {
		Log::Error << "No source attribute provided for channel object" << Log::End;
		return false;
	}

	_source = std::string( ( const char * ) sourceProp ).substr( 1 );
	xmlFree( sourceProp );

	xmlChar *targetProp = xmlGetProp( node, ( xmlChar * ) COLLADA_TARGET );
	if ( !targetProp ) {
		Log::Error << "No target attribute provided for channel object" << Log::End;
		return false;
	}
	
	_target = ( const char * ) targetProp;
	xmlFree( targetProp );

	return true;
}

