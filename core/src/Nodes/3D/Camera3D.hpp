#ifndef CRIMILD_CORE_NODES_3D_CAMERA_
#define CRIMILD_CORE_NODES_3D_CAMERA_

#include "Nodes/3D/Spatial3D.hpp"

namespace crimild::nodes {

   class Camera3D : public Spatial3D {
   public:
      virtual ~Camera3D( void ) = default;

      void lookAt( const Point3f &worldPosition );

      Matrix4f getViewMatrix( void ) const { return Matrix4f( inverse( getWorld() ) ); }

      void setProjectionMatrix( const Matrix4f &projection ) { m_projection = projection; }
      const Matrix4f &getProjectionMatrix( void ) const { return m_projection; }

      Ray3 getPickRay( const Point2 &pointer ) const;

   private:
      Matrix4f m_projection = perspective( 60.0f, 1.0f, 0.1f, 1000.0f );
   };

}

#endif
