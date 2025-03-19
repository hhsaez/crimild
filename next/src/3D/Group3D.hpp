#ifndef CRIMILD_NEXT_3D_GROUP3D_
#define CRIMILD_NEXT_3D_GROUP3D_

#include "3D/Node3D.hpp"

namespace crimild::next {
   
   class Group3D : public Node3D {
      CRIMILD_IMPLEMENT_RTTI( crimild::next::Group3D )
      
   public:
      void attach( std::shared_ptr< Node3D > const &node )
      {
         m_children.push_back( node );
         node->setParent( retain( this ) );
      }
      
      auto begin( void ) const
      {
         return m_children.begin();
      }
      
      auto end( void ) const
      {
         return m_children.end();
      }

      void setLocal( const Transformation &t ) override final
      {
         Node3D::setLocal( t );

         // Update the local transformation of a group results in world
         // transforms for children to be invalidated.
         for ( auto child : m_children ) {
            if ( child != nullptr ) {
               child->setWorldIsCurrent( false );
            }
         }
      }
      
      // void setWorld( const Transformation &t ) override final
      // {
      //    Node3D::setLocal( t );
      //    for ( auto child : m_children ) {
      //       if ( child != nullptr ) {
      //          child->setWorldIsCurrent( false );
      //       }
      //    }
      // }
      
   private:
      std::vector< std::shared_ptr< Node3D > > m_children;
   };
   
}

#endif

