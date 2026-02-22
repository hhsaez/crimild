#ifndef CRIMILD_RENDERING_BINDABLE_MATERIAL_UNLIT_
#define CRIMILD_RENDERING_BINDABLE_MATERIAL_UNLIT_

#include "OpenGLMaterialBindable.hpp"

namespace crimild::opengl {

   class ShaderProgram;

   class UnlitMaterialBindable : public crimild::opengl::MaterialBindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::opengl::UnlitMaterialBindable )

   public:
      virtual ~UnlitMaterialBindable( void ) = default;

      virtual void bind( void ) override;
      virtual void unbind( void ) override;

   protected:
      virtual std::shared_ptr< ShaderProgram > createProgram( void ) override;
   };

}

#endif
