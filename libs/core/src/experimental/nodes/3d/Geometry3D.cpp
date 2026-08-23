#include <crimild/core/experimental/nodes/3d/Geometry3D.hpp>
#include <crimild/core/experimental/nodes/visitors/NodeConstVisitor.hpp>
#include <crimild/core/experimental/nodes/visitors/NodeVisitor.hpp>

using namespace crimild::experimental;

void Geometry3D::accept( NodeVisitor &visitor )
{
   visitor.visitGeometry3D( *this );
}

void Geometry3D::accept( NodeConstVisitor &visitor ) const
{
   visitor.visitGeometry3D( *this );
}
