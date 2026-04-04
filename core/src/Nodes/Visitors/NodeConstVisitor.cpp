#include "Nodes/Visitors/NodeConstVisitor.hpp"

#include "Nodes/3D/Geometry3D.hpp"
#include "Nodes/3D/Group3D.hpp"
#include "Nodes/3D/Spatial3D.hpp"
#include "Nodes/Group.hpp"
#include "Nodes/Node.hpp"

using namespace crimild::experimental;

void NodeConstVisitor::traverse( const Node &node )
{
   node.accept( *this );
}

void NodeConstVisitor::visitNode( const Node &node )
{
   // no-op
}

void NodeConstVisitor::visitGroup( const Group &group )
{
   visitNode( group );
   for ( auto &c : group.getChildren() ) {
      c->accept( *this );
   }
}

void NodeConstVisitor::visitSpatial3D( const Spatial3D &spatial )
{
   visitNode( spatial );
}

void NodeConstVisitor::visitGeometry3D( const Geometry3D &geometry )
{
   visitSpatial3D( geometry );
}

void NodeConstVisitor::visitGroup3D( const Group3D &group )
{
   visitSpatial3D( group );
   for ( auto &c : group.getChildren() ) {
      c->accept( *this );
   }
}
