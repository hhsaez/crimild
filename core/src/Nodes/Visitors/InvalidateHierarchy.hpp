#ifndef CRIMILD_CORE_NODES_VISITORS_INVALIDATE_HIERARCHY_
#define CRIMILD_CORE_NODES_VISITORS_INVALIDATE_HIERARCHY_

#include "Nodes/Visitors/NodeConstVisitor.hpp"

namespace crimild::experimental {

   class InvalidateHierarchy : public NodeVoidConstVisitor {
   public:
      virtual ~InvalidateHierarchy( void ) = default;

      virtual void visitSpatial3D( const Spatial3D & ) override;
   };

}

#endif
