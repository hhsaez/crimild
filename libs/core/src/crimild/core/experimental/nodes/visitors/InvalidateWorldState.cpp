#include <crimild/core/experimental/nodes/3d/Spatial3D.hpp>
#include <crimild/core/experimental/nodes/Node.hpp>
#include <crimild/core/experimental/nodes/visitors/InvalidateWorldState.hpp>

using namespace crimild::experimental;

void InvalidateWorldState::visitSpatial3D( const Spatial3D &spatial )
{
   NodeConstVisitor::visitSpatial3D( spatial );

   spatial.invalidateWorldState();
}
