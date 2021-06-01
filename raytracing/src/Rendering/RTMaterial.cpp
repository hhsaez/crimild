#include "RTMaterial.hpp"

using namespace crimild;
using namespace crimild::raytracing;

RTMaterial::RTMaterial( void )
    : _type( Type::LAMBERTIAN ),
      _albedo( ColorRGB::Constants::WHITE ),
      _fuzz( 0.0f ),
      _refractionIndex( 1.0f )
{
}

RTMaterial::~RTMaterial( void )
{
}
