#include "Nodes/3D/Spatial3D.hpp"

#include "Nodes/Visitors/InvalidateWorldState.hpp"

using namespace crimild;
using namespace crimild::experimental;

std::shared_ptr< Spatial3D > Spatial3D::getParent3D( void ) const
{
   if ( m_parent3D.expired() ) {
      auto owner = getParent();
      while ( owner != nullptr ) {
         if ( auto parent = std::dynamic_pointer_cast< Spatial3D >( owner ) ) {
            m_parent3D = parent;
            perform< InvalidateWorldState >();
            break;
         }
         owner = owner->getParent();
      }
   }

   return !m_parent3D.expired() ? m_parent3D.lock() : nullptr;
}

void Spatial3D::setLocal( const Transformation &local )
{
   m_local = local;
   m_localIsCurrent = true;
   perform< InvalidateWorldState >();
}

const Transformation &Spatial3D::getLocal( void ) const
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

void Spatial3D::setWorld( const Transformation &world )
{
   m_world = world;
   m_localIsCurrent = false;
   perform< InvalidateWorldState >();
   m_worldIsCurrent = true;
}

const Transformation &Spatial3D::getWorld( void ) const
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
