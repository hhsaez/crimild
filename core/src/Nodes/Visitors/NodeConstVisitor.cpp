#include "Nodes/Visitors/NodeConstVisitor.hpp"

#include "Nodes/3D/Geometry3D.hpp"
#include "Nodes/3D/Spatial3D.hpp"
#include "Nodes/Node.hpp"

using namespace crimild::experimental;

void NodeConstVisitor::traverse( const Node &node )
{
   node.accept( *this );
}

void NodeConstVisitor::visitNode( const Node &node )
{
   for ( auto &c : node.getChildren() ) {
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
