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
