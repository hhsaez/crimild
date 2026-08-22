#ifndef CRIMILD_CORE_NODES_VISITORS_INVALIDATE_WORLD_STATE_
#define CRIMILD_CORE_NODES_VISITORS_INVALIDATE_WORLD_STATE_

#include <crimild/core/Nodes/Visitors/NodeConstVisitor.hpp>

namespace crimild::experimental {

   class InvalidateWorldState : public NodeVoidConstVisitor {
   public:
      virtual ~InvalidateWorldState( void ) = default;

      virtual void visitSpatial3D( const Spatial3D & ) override;
   };

}

#endif
