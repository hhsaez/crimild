#ifndef CRIMILD_CORE_NODES_3D_SPATIAL_
#define CRIMILD_CORE_NODES_3D_SPATIAL_

#include "Common/Signal.hpp"
#include "Nodes/Node.hpp"

namespace crimild::nodes {

   /**
    * @brief An entity representing a 3D object
    */
   class Spatial3D : public Node {
   public:
      Signal<> worldChanged;

   public:
      virtual ~Spatial3D( void ) = default;

      inline bool hasParent3D( void ) const { return !m_parent3D.expired(); }

      std::shared_ptr< Spatial3D > getParent3D( void ) const
      {
         if ( m_parent3D.expired() ) {
            auto owner = getParent();
            while ( owner != nullptr ) {
               if ( auto parent = std::dynamic_pointer_cast< Spatial3D >( owner ) ) {
                  m_parent3D = parent;
                  auto self = retain( const_cast< Spatial3D * >( this ) );
                  self->invalidateWorld();
                  parent->worldChanged.bind(
                     self,
                     &Spatial3D::invalidateWorld
                  );
                  break;
               }
               owner = owner->getParent();
            }
         }

         return !m_parent3D.expired() ? m_parent3D.lock() : nullptr;
      }

      inline bool isLocalCurrent( void ) const { return m_localIsCurrent; }

      void setLocal( const Transformation &local )
      {
         m_local = local;
         m_localIsCurrent = true;
         invalidateWorld();
      }

      const Transformation &getLocal( void ) const
      {
         if ( !m_localIsCurrent ) {
            if ( auto parent = getParent3D() ) {
               // When a parent is present, world is computed as W = P * L
               // so local can be computed as L = inv(P) * W
               m_local = inverse( parent->getWorld() )( m_world );
            } else {
               // No parent: World and Local are the same transformation
               m_local = m_world;
            }
            m_localIsCurrent = true;
         }
         return m_local;
      }

      inline bool isWorldCurrent( void ) const { return m_worldIsCurrent; }

      void setWorld( const Transformation &world )
      {
         m_world = world;
         m_localIsCurrent = false;
         m_worldIsCurrent = true;
         worldChanged();
      }

      const Transformation &getWorld( void ) const
      {
         if ( !m_worldIsCurrent ) {
            if ( auto parent = getParent3D() ) {
               m_world = parent->getWorld()( m_local );
            } else {
               m_world = m_local;
            }
            m_worldIsCurrent = true;
         }
         return m_world;
      }

   protected:
      virtual void onParentChanged( void ) override
      {
         Node::onParentChanged();
         if ( auto parent = m_parent3D.lock() ) {
            parent->worldChanged.unbind( retain( this ) );
         }
         m_parent3D.reset();
         invalidateWorld();
      }

   private:
      void invalidateWorld( void )
      {
         m_worldIsCurrent = false;
         worldChanged();
      }

   private:
      mutable std::weak_ptr< Spatial3D > m_parent3D;
      mutable Transformation m_local = Transformation::Constants::IDENTITY;
      mutable bool m_localIsCurrent = true;
      mutable Transformation m_world = Transformation::Constants::IDENTITY;
      mutable bool m_worldIsCurrent = true;
   };

}

#endif
