#include "Nodes/3D/Camera3D.hpp"

using namespace crimild::experimental;

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
   // Implements a perspective-independent approach to compute the picking ray
   // This is tricky because, for perspective project, the ray origin is the same
   // and the direction varies depending on pointer. On the contrary, for orthographic
   // projections, the origin changes depending on pointer while the direction
   // remains the same.

   // Convert normalized pointer to NDC
   const float x = 2.0f * pointer.x - 1.0f;
   const float y = 1.0f - 2.0f * pointer.y;

   // Compute inverse view-projection
   const auto invViewProj = inverse( getProjectionMatrix() * getViewMatrix() );

#if CRIMILD_USE_DEPTH_RANGE_ZERO_TO_ONE
   const auto zNear = 0.0f;
   const auto zFar = 1.0f;
#else
   const auto zNear = -1.0f;
   const auto zFar = 1.0f;
#endif

   // Near/Far at clip space
   const auto clipNear = Vector4 { x, y, zNear, 1 };
   const auto clipFar = Vector4 { x, y, zFar, 1 };

   // Unproject clip near/far. Result is homogeneous world space
   const auto worldHNear = invViewProj * clipNear;
   const auto worldHFar = invViewProj * clipFar;

   // Do perspective divide on each point
   const auto worldNear = Point3( worldHNear / worldHNear.w );
   const auto worldFar = Point3( worldHFar / worldHFar.w );

   // For perspective projection, worldNear and worldFar lie along a ray that
   // starts at the camera/eye position, so using the camera origin as the ray
   // origin is also valid for many picking tests. But using worldNear as the
   // ray origin is still fine and often preferable because it starts exactly
   // at the near plane.
   // For orthographic projection, worldNear is essential: different pointer
   // positions produce different ray origins. The direction from worldNear
   // to worldFar will be the same for every pointer poisition, matching
   // ortographic projection.
   const auto rayOrigin = worldNear;
   const auto rayDirection = normalize( worldFar - worldNear );

   return Ray3 {
      rayOrigin,
      rayDirection,
   };
}
