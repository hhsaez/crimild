#include <algorithm>
#include <crimild/core/experimental/behaviors/composites/Composite.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

Composite::~Composite( void ) noexcept
{
   for ( const auto &behavior : m_behaviors ) {
      behavior->setParent( nullptr );
   }
}

void Composite::attach( std::shared_ptr< Behavior > const &behavior )
{
   // Keep a copy: callers may pass a reference into our child collection.
   const auto child = behavior;
   if ( !canAttach( child.get() ) ) {
      return;
   }
   abort();
   // Cleanup callbacks may have changed ownership.
   if ( !canAttach( child.get() ) ) {
      return;
   }
   child->detachFromParent();
   // Do not override a different ownership decision made by cleanup callbacks.
   if ( !canAttach( child.get() ) || child->hasParent() ) {
      return;
   }
   m_behaviors.push_back( child );
   child->setParent( this );
}

void Composite::detach( std::shared_ptr< Behavior > const &behavior )
{
   const auto child = behavior;
   // Inspect the non-owning link without retaining a parent under construction or cleanup.
   if ( child == nullptr || child->m_parent != this ) {
      return;
   }
   abort();
   if ( child->m_parent != this ) {
      return;
   }
   child->abort();
   // Cleanup callbacks may already have detached it.
   if ( child->m_parent != this ) {
      return;
   }
   std::erase( m_behaviors, child );
   child->setParent( nullptr );
}

void Composite::terminate( State state )
{
   // Keep callbacks that modify the collection from invalidating traversal.
   const auto behaviors = getBehaviors();
   for ( const auto &behavior : behaviors ) {
      if ( behavior->m_parent == this ) {
         behavior->abort();
      }
   }
}
