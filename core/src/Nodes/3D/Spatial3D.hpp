#ifndef CRIMILD_CORE_NODES_3D_SPATIAL_
#define CRIMILD_CORE_NODES_3D_SPATIAL_

#include "Common/Signal.hpp"
#include "Nodes/Node.hpp"

namespace crimild::experimental {

   /**
    * @brief An entity representing a 3D object
    *
    * When to invalidate world state?
    * - local changed
    * - parent/hierarhcy changed
    * - parent world changed
    * - world change invalidates child world state (self is current)
    *
    * World invalidation appens on explicit hiearchy/local/world mutations,
    * not on lazy recomputation via getWorld()
    */
   class Spatial3D : public Node {
   public:
      Signal<> worldChanged;

   public:
      virtual ~Spatial3D( void ) = default;

      inline bool hasParent3D( void ) const { return !m_parent3D.expired(); }

      /**
       * Returns the closest Spatial3D ancestor, whether direct or
       * indirect through non-Spatial3D nodes.
       */
      std::shared_ptr< Spatial3D > getParent3D( void ) const;

      inline bool isLocalCurrent( void ) const { return m_localIsCurrent; }

      /**
       * @remarks Invalidates world state
       */
      void setLocal( const Transformation &local );
      const Transformation &getLocal( void ) const;

      inline bool isWorldCurrent( void ) const { return m_worldIsCurrent; }

      /**
       * @remarks Invalidates local state
       */
      void setWorld( const Transformation &world );
      const Transformation &getWorld( void ) const;

      /*
      void invalidateParent3D( void )
      {
         m_parent3D.reset();
         invalidateWorldState();
      }

      void invalidateWorldState( void )
      {
         m_worldIsCurrent = false;
         worldChanged();
         }
         */

   protected:
      friend class InvalidateHierarchy;
      inline void invalidateParent3D( void ) const { m_parent3D.reset(); }

      friend class InvalidateWorldState;
      inline void invalidateWorldState( void ) const { m_worldIsCurrent = false; }
      /*
   virtual void onParentChanged( void ) override
   {
      Node::onParentChanged();
      if ( auto parent = m_parent3D.lock() ) {
         parent->worldChanged.unbind( retain( this ) );
      }
      m_parent3D.reset();
      invalidateWorld();
      }
      */

   private:
      mutable std::weak_ptr< Spatial3D > m_parent3D;
      mutable Transformation m_local = Transformation::Constants::IDENTITY;
      mutable bool m_localIsCurrent = true;
      mutable Transformation m_world = Transformation::Constants::IDENTITY;
      mutable bool m_worldIsCurrent = true;
   };

}

#endif
