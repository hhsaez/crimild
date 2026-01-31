#ifndef CRIMILD_CORE_NODES_NODE
#define CRIMILD_CORE_NODES_NODE

#include "Common/Signal.hpp"
#include "Entity/Entity.hpp"

#include <Crimild_Foundation.hpp>

namespace crimild::nodes {

   /**
    * @brief A named entity that is part of a hierarchy
    *
    * Nodes should have up to one parent and may have any number of children.
    */
   class Node
      : public Entity,
        public Named {
   public:
   public:
      Signal<> parentChanged;

   public:
      virtual ~Node( void ) = default;

      inline bool hasParent( void ) const
      {
         return !m_parent.expired();
      }

      std::shared_ptr< Node > getParent( void ) const
      {
         return m_parent.lock();
      }

      void setParent( std::shared_ptr< Node > const &newParent )
      {
         if ( auto parent = m_parent.lock() ) {
            parent->parentChanged.unbind( retain( this ) );
         }
         m_parent = newParent;
         if ( auto parent = m_parent.lock() ) {
            parent->parentChanged.bind(
               retain( this ),
               &Node::onParentChanged
            );
         }
      }

      std::shared_ptr< Node > detachFromParent( void )
      {
         // Since we're detaching from our owner, keep a retained pointer so we're
         // not destroyed yet.
         auto self = retain( this );
         if ( auto parent = getParent() ) {
            parent->detach( self );
         }
         return self;
      }

      void attach( std::shared_ptr< Node > const &child )
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

      void detach( std::shared_ptr< Node > const &child )
      {
         if ( child == nullptr ) {
            return;
         }

         if ( child->getParent() != nullptr ) {
            return;
         }

         auto it = std::find( m_children.begin(), m_children.end(), child );
         m_children.erase( it );
         child->setParent( nullptr );
      }

      const std::vector< std::shared_ptr< Node > > &getChildren( void ) const
      {
         return m_children;
      }

   protected:
      virtual void onParentChanged( void )
      {
         parentChanged();
      }

   private:
      std::weak_ptr< Node > m_parent;
      std::vector< std::shared_ptr< Node > > m_children;
   };

}

#endif
