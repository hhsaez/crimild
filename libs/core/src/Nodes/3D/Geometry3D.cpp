#include <crimild/core/Nodes/3D/Geometry3D.hpp>

#include <crimild/core/Nodes/Visitors/NodeConstVisitor.hpp>
#include <crimild/core/Nodes/Visitors/NodeVisitor.hpp>

using namespace crimild::experimental;

void Geometry3D::accept( NodeVisitor &visitor )
{
   visitor.visitGeometry3D( *this );
}

void Geometry3D::accept( NodeConstVisitor &visitor ) const
{
   visitor.visitGeometry3D( *this );
}
