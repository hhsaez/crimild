#ifndef CRIMILD_RENDERING_BINDABLE_MATERIAL_
#define CRIMILD_RENDERING_BINDABLE_MATERIAL_

#include "OpenGLBindable.hpp"

namespace crimild::opengl {

   class ShaderProgram;

   class MaterialBindable : public crimild::opengl::Bindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::opengl::MaterialBindable )

   public:
      virtual ~MaterialBindable( void ) = default;

      virtual void bind( void ) override;
      virtual void unbind( void ) override;

      inline const std::shared_ptr< ShaderProgram > &getProgram( void ) const { return m_program; }

   protected:
      virtual std::shared_ptr< ShaderProgram > createProgram( void ) = 0;

   private:
      std::shared_ptr< ShaderProgram > m_program;
   };

}

#endif
