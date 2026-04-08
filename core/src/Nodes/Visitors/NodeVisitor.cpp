#include "Nodes/Visitors/NodeVisitor.hpp"

#include "Nodes/3D/Geometry3D.hpp"
#include "Nodes/3D/Spatial3D.hpp"
#include "Nodes/Node.hpp"

using namespace crimild::experimental;

void NodeVisitor::traverse( Node &node )
{
   node.accept( *this );
}

void NodeVisitor::visitNode( Node &node )
{
   for ( auto &c : node.getChildren() ) {
      c->accept( *this );
   }
}

void NodeVisitor::visitSpatial3D( Spatial3D &spatial )
{
   visitNode( spatial );
}

void NodeVisitor::visitGeometry3D( Geometry3D &geometry )
{
   visitSpatial3D( geometry );
}
