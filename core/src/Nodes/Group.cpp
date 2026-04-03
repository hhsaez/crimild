#include "Nodes/Group.hpp"

#include "Nodes/Visitors/NodeConstVisitor.hpp"
#include "Nodes/Visitors/NodeVisitor.hpp"

using namespace crimild::experimental;

void Group::accept( NodeVisitor &visitor )
{
   visitor.visitGroup( *this );
}

void Group::accept( NodeConstVisitor &visitor ) const
{
   visitor.visitGroup( *this );
}
