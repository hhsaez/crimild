#ifndef CRIMILD_RENDERING_BINDABLE_MATERIAL_UNLIT_
#define CRIMILD_RENDERING_BINDABLE_MATERIAL_UNLIT_

#include "OpenGLMaterialBindable.hpp"

#include <memory>

namespace crimild {

   class Texture;

}

namespace crimild::universal {

   class ShaderProgram;

   class UnlitMaterialBindable : public MaterialBindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::universal::UnlitMaterialBindable )

   public:
      virtual ~UnlitMaterialBindable( void ) = default;

      virtual void bind( void ) override;
      virtual void unbind( void ) override;
      virtual void unload( void ) override;

   protected:
      virtual std::shared_ptr< ShaderProgram > createProgram( void ) override;

   private:
      std::weak_ptr< Texture > m_boundTexture;
   };

}

#endif
