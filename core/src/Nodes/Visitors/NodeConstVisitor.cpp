#include "Nodes/Visitors/NodeConstVisitor.hpp"

#include "Nodes/3D/Group3D.hpp"
#include "Nodes/3D/Spatial3D.hpp"
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

void NodeConstVisitor::visitSpatial3D( const Spatial3D &spatial )
{
   visitNode( spatial );
}

void NodeConstVisitor::visitGroup3D( const Group3D &group )
{
   visitSpatial3D( group );
   for ( auto &c : group.getChildren() ) {
      c->accept( *this );
   }
}
