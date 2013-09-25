#include "Skin.hpp"

using namespace crimild;
using namespace crimild::collada;

Skin::Skin( void )
{

}

Skin::~Skin( void )
{

}

bool Skin::parseXML( xmlNode *input )
{
	Log::Debug << "Parsing <skin> object" << Log::End;

	xmlChar *sourceProp = xmlGetProp( input, ( xmlChar * ) COLLADA_SOURCE );
	if ( !sourceProp ) {
		Log::Error << "No source attribute provided for skin object" << Log::End;
		return false;
	}

	_sourceID = std::string( ( const char * ) sourceProp ).substr( 1 );
	xmlFree( sourceProp );

	_sources.parseXML( input );

	xmlNode *jointsXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_JOINTS );
	if ( jointsXML ) {
		JointsPtr joints( new Joints() );
		if ( joints->parseXML( jointsXML ) ) {
			_joints = joints;
		}
	}

	xmlNode *vertexWeightsXML = XMLUtils::getChildXMLNodeWithName( input, COLLADA_VERTEX_WEIGHTS );
	if ( vertexWeightsXML ) {
		VertexWeightsPtr vertexWeights( new VertexWeights() );
		if ( vertexWeights->parseXML( vertexWeightsXML ) ) {
			_vertexWeights = vertexWeights;
		}
	}

	return true;
}

