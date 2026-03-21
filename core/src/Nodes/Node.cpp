#include "Nodes/Node.hpp"

using namespace crimild::nodes;

std::shared_ptr< Node > Node::detachFromParent( void )
{
   // Since we're detaching from our owner, keep a retained pointer so we're
   // not destroyed yet.
   auto self = retain( this );
   if ( auto parent = getParent() ) {
      parent->detach( self );
   }
   return self;
}

void Node::attach( std::shared_ptr< Node > const &child )
{
   if ( child == nullptr ) {
      return;
   }

   if ( child->getParent().get() == this ) {
      return;
   }

   child->detachFromParent();
   m_children.push_back( child );
   child->setParent( retain( this ) );
}

void Node::detach( std::shared_ptr< Node > const &child )
{
   if ( child == nullptr ) {
      return;
   }

   if ( child->getParent().get() != this ) {
      return;
   }

   auto it = std::find( m_children.begin(), m_children.end(), child );
   m_children.erase( it );
   child->setParent( nullptr );
}

void Node::setParent( std::shared_ptr< Node > const &parent )
{
   m_parent = parent;
}
