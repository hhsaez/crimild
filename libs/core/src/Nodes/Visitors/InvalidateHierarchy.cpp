#include <crimild/core/Nodes/Visitors/InvalidateHierarchy.hpp>

#include <crimild/core/Nodes/3D/Spatial3D.hpp>

using namespace crimild::experimental;

void InvalidateHierarchy::visitSpatial3D( const Spatial3D &spatial )
{
   NodeConstVisitor::visitSpatial3D( spatial );

   spatial.invalidateParent3D();
   spatial.invalidateWorldState();
}
