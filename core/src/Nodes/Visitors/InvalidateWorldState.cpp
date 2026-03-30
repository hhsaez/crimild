#include "Nodes/Visitors/InvalidateWorldState.hpp"

#include "Nodes/3D/Spatial3D.hpp"
#include "Nodes/Node.hpp"

using namespace crimild::experimental;

void InvalidateWorldState::visitSpatial3D( const Spatial3D &spatial )
{
   NodeConstVisitor::visitSpatial3D( spatial );

   spatial.invalidateWorldState();
}
