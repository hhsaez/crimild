#include "RTMaterial.hpp"

using namespace crimild;
using namespace crimild::raytracing;

RTMaterial::RTMaterial( void )
	: _albedo( RGBColorf::ONE ),
	  _type( Type::LAMBERTIAN ),
	  _fuzz( 0.0f ),
	  _refractionIndex( 1.0f )
{
	
}

RTMaterial::~RTMaterial( void )
{
	
}

