#ifndef CRIMILD_CORE_NODES_NODE
#define CRIMILD_CORE_NODES_NODE

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
      virtual ~Node( void ) = default;

      inline bool hasParent( void ) const
      {
         return !m_parent.expired();
      }

      std::shared_ptr< Node > getParent( void ) const
      {
         return m_parent.lock();
      }

      template< class NodeType >
      std::shared_ptr< NodeType > getParent( void ) const
      {
         return std::static_pointer_cast< NodeType >( m_parent.lock() );
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

         if ( child->getParent().get() != this ) {
            return;
         }

         auto it = std::find( m_children.begin(), m_children.end(), child );
         m_children.erase( it );
         child->setParent( nullptr );
      }

      [[nodiscard]] bool hasChild( std::shared_ptr< Node > const &child ) const
      {
         return std::find( m_children.begin(), m_children.end(), child ) != m_children.end();
      }

      template< class NodeType >
      std::shared_ptr< NodeType > getChildAt( size_t index ) const
      {
         if ( index >= m_children.size() ) {
            return nullptr;
         }
         return std::static_pointer_cast< NodeType >( m_children.at( index ) );
      }

      const std::vector< std::shared_ptr< Node > > &getChildren( void ) const
      {
         return m_children;
      }

   protected:
      virtual void onParentChanged( void )
      {
         for ( auto &maybeChild : m_children ) {
            if ( auto child = maybeChild ) {
               child->onParentChanged();
            }
         }
      }

   private:
      void setParent( std::shared_ptr< Node > const &newParent )
      {
         m_parent = newParent;
         onParentChanged();
      }

   private:
      std::weak_ptr< Node > m_parent;
      std::vector< std::shared_ptr< Node > > m_children;
   };

}

#endif
