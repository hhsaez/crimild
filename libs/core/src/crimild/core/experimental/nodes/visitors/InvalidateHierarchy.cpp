#include <crimild/core/experimental/nodes/3d/Spatial3D.hpp>
#include <crimild/core/experimental/nodes/visitors/InvalidateHierarchy.hpp>

using namespace crimild::experimental;

void InvalidateHierarchy::visitSpatial3D( const Spatial3D &spatial )
{
   NodeConstVisitor::visitSpatial3D( spatial );

   spatial.invalidateParent3D();
   spatial.invalidateWorldState();
}
