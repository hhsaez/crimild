#include "Nodes/3D/Group3D.hpp"

#include "Nodes/Visitors/NodeConstVisitor.hpp"
#include "Nodes/Visitors/NodeVisitor.hpp"

using namespace crimild::experimental;

void Group3D::accept( NodeVisitor &visitor )
{
   visitor.visitGroup3D( *this );
}

void Group3D::accept( NodeConstVisitor &visitor ) const
{
   visitor.visitGroup3D( *this );
}
