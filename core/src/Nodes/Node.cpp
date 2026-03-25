#include "Nodes/Node.hpp"

#include "Nodes/Group.hpp"

#include <memory>

using namespace crimild::experimental;

std::shared_ptr< Node > Node::detachFromParent( void )
{
   // Since we're detaching from our owner, keep a retained pointer so we're
   // not destroyed yet.
   auto self = retain( this );
   if ( hasParent() ) {
      if ( auto parent = std::dynamic_pointer_cast< Group >( getParent() ) ) {
         parent->detach( self );
      }
   }
   return self;
}

void Node::setParent( std::shared_ptr< Node > const &parent )
{
   m_parent = parent;
}
