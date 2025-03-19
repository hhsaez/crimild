#ifndef CRIMILD_NEXT_3D_TRANSFORM3D_
#define CRIMILD_NEXT_3D_TRANSFORM3D_

#include "3D/Node.hpp"

namespace crimild::next {

   /**
    * @brief A node that can be transformed in 3D space
    */
   class Transform3D : public Node3D {
      CRIMILD_IMPLEMENT_RTTI( crimild::next::Transform3D )

   public:
      void setLocal( const Transformation &t )
      {
         m_local = t;
         m_localIsCurrent = true;
         m_worldIsCurrent = false;
      }

      [[nodiscard]] const Transformation &getLocal( void ) const
      {
         if ( !m_localIsCurrent ) {
            // TODO
         }
         return m_local;
      }

      void setWorldNeedsUpdate( void ) final
      {
         // TODO
      }

      virtual const Transformation &getWorld( void ) const override
      {
         return Node3D::getWorld();
      }

      void setChild( std::shared_ptr< Node3D > const &child )
      {
         m_child = child;
         child->setParent( retain( this ) );
      }

      std::shared_ptr< Node3D > getChild( void ) const
      {
         return m_child;
      }

   private:
      std::shared_ptr< Node3D > m_child;
   };

}

#endif
