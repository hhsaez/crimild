#ifndef CRIMILD_RENDERING_BINDABLE_MATERIAL_
#define CRIMILD_RENDERING_BINDABLE_MATERIAL_

#include "OpenGLBindable.hpp"

namespace crimild::opengl {

   class ShaderProgram;

   class MaterialBindable : public crimild::opengl::Bindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::opengl::MaterialBindable )

   public:
      virtual ~MaterialBindable( void );

      virtual void bind( void ) override;
      virtual void unbind( void ) override;

   private:
      void load( void );
      void unload( void );

   private:
      std::shared_ptr< ShaderProgram > m_program;
   };

}

#endif
