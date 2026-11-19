#ifndef CRIMILD_CORE_EXPERIMENTAL_NODES_3D_BOUNDING_VOLUME_
#define CRIMILD_CORE_EXPERIMENTAL_NODES_3D_BOUNDING_VOLUME_

#include <crimild/core/Common/Signal.hpp>
#include <crimild/core/experimental/nodes/3d/Spatial3D.hpp>

namespace crimild::experimental {

   class BoundingVolume3D : public Spatial3D {
   public:
      crimild::Signal< BoundingVolume3D &, BoundingVolume3D & > intersectionBegan;
      crimild::Signal<> intersected;
      crimild::Signal<> intersectionEnded;

   public:
      virtual ~BoundingVolume3D( void ) noexcept = default;
   };

   class SphereBoundingVolume3D : public BoundingVolume3D {
   public:
      virtual ~SphereBoundingVolume3D( void ) noexcept = default;
   };

}

#endif
