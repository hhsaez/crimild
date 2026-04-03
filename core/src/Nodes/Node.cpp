#include "Nodes/Node.hpp"

#include "Nodes/Visitors/InvalidateHierarchy.hpp"
#include "Nodes/Visitors/NodeConstVisitor.hpp"
#include "Nodes/Visitors/NodeVisitor.hpp"

#include <memory>

using namespace crimild::experimental;

std::shared_ptr< Node > Node::detachFromParent( void )
{
   // Since we're detaching from our owner, keep a retained pointer so we're
   // not destroyed yet.
   auto self = retain( this );
   if ( auto parent = std::dynamic_pointer_cast< ParentNode >( getParent() ) ) {
      parent->detach( self );
   }
   return self;
}

void Node::setParent( std::shared_ptr< Node > const &parent )
{
   assert( parent == nullptr || std::dynamic_pointer_cast< ParentNode >( parent ) != nullptr );
   m_parent = parent;
   perform< InvalidateHierarchy >();
}

void Node::accept( NodeVisitor &visitor )
{
   visitor.visitNode( *this );
}

void Node::accept( NodeConstVisitor &visitor ) const
{
   visitor.visitNode( *this );
}
