#include "Nodes/3D/Camera3D.hpp"

using namespace crimild::nodes;

void Camera3D::lookAt( const Point3f &worldPosition )
{
   setLocal(
      crimild::lookAt(
         origin( getLocal() ),
         inverse( getWorld() )( worldPosition ),
         Vector3f::Constants::UP
      )
   );
}

crimild::Ray3 Camera3D::getPickRay( const Point2 &pointer ) const
{
   const float x = 2.0f * pointer.x - 1.0f;
   const float y = 1.0f - 2.0f * pointer.y;

   const auto rayClip = Vector4 { x, y, -1.0f, 1.0f };

   auto rayEye = inverse( getProjectionMatrix() ) * rayClip;
   const auto rayOrigin = origin( getWorld() );
   const auto rayDirection = normalize( getWorld()( Vector3( rayEye ) ) );

   return Ray3 {
      rayOrigin,
      rayDirection,
   };
}
