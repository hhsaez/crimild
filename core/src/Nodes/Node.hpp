#ifndef CRIMILD_CORE_NODES_NODE_
#define CRIMILD_CORE_NODES_NODE_

#include "Entity/Entity.hpp"

#include <Crimild_Foundation.hpp>

namespace crimild::experimental {

   /**
    * @brief A named entity that is part of a hierarchy
    *
    * Nodes form a hierarchy but the base class only defines the parent relationship.
    *
    * Why not including child management in Node?
    * The reason is that different nodes may handle children in different ways. In the
    * simplest case, a Group, we have a bunch of child nodes (sorted or not, it doesn't
    * really matter). But most complex nodes, like a Switch, LOD, Decorators, may have
    * one, two or any kind of children and some of them might not be active at any
    * given point in time.
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

      std::shared_ptr< Node > detachFromParent( void );

      private:
      // Use friendship to declare which classes have access to internal engine functions.
      // This list should not change frequently considering we should only need to explicitly
      // declare the mixins as friends.
         template< class BaseType, class ChildType > friend class WithChildren;
      void setParent( std::shared_ptr< Node > const &newParent );

   private:
      std::weak_ptr< Node > m_parent;
   };

}

#endif
