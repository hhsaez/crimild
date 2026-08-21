#include <crimild/core/Nodes/Visitors/InvalidateWorldState.hpp>

#include <crimild/core/Nodes/3D/Spatial3D.hpp>
#include <crimild/core/Nodes/Node.hpp>

using namespace crimild::experimental;

void InvalidateWorldState::visitSpatial3D( const Spatial3D &spatial )
{
   NodeConstVisitor::visitSpatial3D( spatial );

   spatial.invalidateWorldState();
}
