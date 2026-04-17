#ifndef CRIMILD_CORE_NODES_NODE_
#define CRIMILD_CORE_NODES_NODE_

#include "Entity/Entity.hpp"

#include <Crimild_Foundation.hpp>
#include <type_traits>

namespace crimild::experimental {

   class Node;
   class NodeVisitor;
   class NodeConstVisitor;

   template< class T >
   concept IsNode = std::is_base_of_v< Node, T >;

   /**
    * @brief A named entity that is part of a hierarchy
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

      template< IsNode ParentNodeType >
      std::shared_ptr< ParentNodeType > getParent( void ) const
      {
         return std::static_pointer_cast< ParentNodeType >( m_parent.lock() );
      }

      std::shared_ptr< Node > detachFromParent( void );

   private:
      void setParent( std::shared_ptr< Node > const &newParent );

   private:
      std::weak_ptr< Node > m_parent;

   public:
      void attach( std::shared_ptr< Node > const &child );

      template< IsNode NodeType, typename... Args >
      std::shared_ptr< NodeType > attach( Args &&...args )
      {
         auto child = std::make_shared< NodeType >( std::forward< Args >( args )... );
         attach( child );
         return child;
      }

      void detach( std::shared_ptr< Node > const &child );

      [[nodiscard]] bool hasChild( std::shared_ptr< Node > const &child ) const
      {
         return std::find( m_children.begin(), m_children.end(), child ) != m_children.end();
      }

      template< class T >
      std::shared_ptr< T > getChildAt( size_t index ) const
      {
         if ( index >= m_children.size() ) {
            return nullptr;
         }
         return std::static_pointer_cast< T >( m_children.at( index ) );
      }

      /**
       * @brief Get node children
       *
       * Returns a const reference to prevent direct modifications to the children array
       */
      const std::vector< std::shared_ptr< Node > > &getChildren( void ) const
      {
         return m_children;
      }

   private:
      std::vector< std::shared_ptr< Node > > m_children;

   public:
      template< class VisitorType, typename... Args >
      VisitorType::ResultType perform( Args &&...args )
      {
         VisitorType visitor( std::forward< Args >( args )... );
         visitor.traverse( *this );
         if constexpr ( !std::is_void_v< typename VisitorType::ResultType > ) {
            return visitor.getResult();
         }
      }

      template< class ConstVisitorType, typename... Args >
      ConstVisitorType::ResultType perform( Args &&...args ) const
      {
         ConstVisitorType visitor( std::forward< Args >( args )... );
         visitor.traverse( *this );
         if constexpr ( !std::is_void_v< typename ConstVisitorType::ResultType > ) {
            return visitor.getResult();
         }
      }

      virtual void accept( NodeVisitor &visitor );
      virtual void accept( NodeConstVisitor &visitor ) const;
   };

}

#endif
