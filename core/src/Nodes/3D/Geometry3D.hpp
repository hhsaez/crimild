#ifndef CRIMILD_CORE_NODES_3D_GEOMETRY_
#define CRIMILD_CORE_NODES_3D_GEOMETRY_

#include "Nodes/3D/Spatial3D.hpp"

namespace crimild {

   class Primitive;
   class Material;

}

namespace crimild::nodes {

   class Geometry3D : public Spatial3D {
   public:
      virtual ~Geometry3D( void ) = default;

      const std::shared_ptr< Primitive > &getPrimitive( void ) const { return m_primitive; }
      std::shared_ptr< Primitive > &getPrimtive( void ) { return m_primitive; }
      void setPrimitive( std::shared_ptr< Primitive > const &primitive ) { m_primitive = primitive; }

      const std::shared_ptr< Material > &getMaterial( void ) const { return m_material; }
      std::shared_ptr< Material > &getMaterial( void ) { return m_material; }

      template< class MaterialType >
      std::shared_ptr< MaterialType > getMaterial( void )
      {
         return static_pointer_cast< MaterialType >( m_material );
      }

      void setMaterial( std::shared_ptr< Material > const &material ) { m_material = material; }

   private:
      std::shared_ptr< Primitive > m_primitive;
      std::shared_ptr< Material > m_material;
   };

}

#endif
