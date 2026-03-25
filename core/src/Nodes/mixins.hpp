#ifndef CRIMILD_CORE_NODES_MIXINS_
#define CRIMILD_CORE_NODES_MIXINS_

#include "Nodes/Node.hpp"

namespace crimild::experimental {

   template<
      class Base,
      class ChildType = Node >
   class WithChildren : public Base {
   public:
      void attach( std::shared_ptr< ChildType > const &child )
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

      void detach( std::shared_ptr< ChildType > const &child )
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

      [[nodiscard]] bool hasChild( std::shared_ptr< ChildType > const &child ) const
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
      const std::vector< std::shared_ptr< ChildType > > &getChildren( void ) const
      {
         return m_children;
      }

   private:
      std::vector< std::shared_ptr< ChildType > > m_children;
   };

}

#endif
