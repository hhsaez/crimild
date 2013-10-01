#include "Texture.hpp"

using namespace crimild;
using namespace crimild::collada;

collada::Texture::Texture( void )
{

}

collada::Texture::~Texture( void )
{

}

bool collada::Texture::parseXML( xmlNode *input )
{
	XMLUtils::getAttribute( input, COLLADA_TEXTURE, _texture );
	XMLUtils::getAttribute( input, COLLADA_TEXCOORD, _texcoord );

	return true;
}

