#include "Source.hpp"

using namespace crimild;
using namespace crimild::collada;

Source::Source( void )
{
	_count = 0;
}

Source::~Source( void )
{

}

bool Source::parseXML( xmlNode *input )
{
	xmlChar *idProp = xmlGetProp( input, ( xmlChar * ) COLLADA_ID );
	if ( !idProp ) {
		Log::Error << "No id attribute provided for sampler object" << Log::End;
		return false;
	}

	setID( ( const char * ) idProp );
	xmlFree( idProp );

	Log::Debug << "Parsing <source> object: " << getID() << Log::End;

	// attempt to load float values in the source
	xmlNode *floatArray = XMLUtils::getChildXMLNodeWithName( input, COLLADA_FLOAT_ARRAY );
	if ( floatArray ) {
		xmlChar *arrayCount = xmlGetProp( floatArray, ( const xmlChar * ) COLLADA_COUNT );
		if ( arrayCount ) {
			_count = xmlStringToValue< int >( arrayCount );
			if ( _count > 0 ) {
				xmlChar *content = xmlNodeGetContent( floatArray );
				if ( content ) {
					std::stringstream str;
					str << content;
					_floatArray.resize( _count );
					float *data = &_floatArray[ 0 ];
					for ( unsigned int i = 0; i < _count; i++ ) {
						str >> data[ i ];
					}
					xmlFree( content );
				}
			}
			else {
				Log::Warning << "No data found in source" << Log::End;
			}
		}
		else {
			Log::Warning << "Cannot obtain 'count' property from array" << Log::End;
		}
		xmlFree( arrayCount );
	}

	// attempt to load name values in the source
	xmlNode *nameArray = XMLUtils::getChildXMLNodeWithName( input, COLLADA_NAME_ARRAY );
	if ( nameArray ) {
		xmlChar *arrayCount = xmlGetProp( nameArray, ( const xmlChar * ) COLLADA_COUNT );
		if ( arrayCount ) {
			_count = xmlStringToValue< int >( arrayCount );
			if ( _count > 0 ) {
				xmlChar *content = xmlNodeGetContent( nameArray );
				if ( content ) {
					std::stringstream str;
					str << content;
					std::string temp;
					for ( unsigned int i = 0; i < _count; i++ ) {
						str >> temp;
						_nameArray.push_back( temp );
					}
					xmlFree( content );
				}
			}
			else {
				Log::Warning << "No data found in source" << Log::End;
			}
		}
		else {
			Log::Warning << "Cannot obtain 'count' property from array" << Log::End;
		}
		xmlFree( arrayCount );
	}

	return true;
}

