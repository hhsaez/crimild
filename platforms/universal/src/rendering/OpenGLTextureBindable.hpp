#ifndef CRIMILD_UNIVERSAL_RENDERING_BINDABLE_TEXTURE_
#define CRIMILD_UNIVERSAL_RENDERING_BINDABLE_TEXTURE_

#include "rendering/OpenGLBindable.hpp"

#include <crimild/universal/foundation/GLUtils.hpp>

namespace crimild::universal {

   class TextureBindable : public Bindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::universal::TextureBindable )

   public:
      virtual ~TextureBindable( void ) noexcept;

      virtual void bind( void ) override;
      virtual void unbind( void ) override;
      virtual void unload( void ) override;

   private:
      void load( void );

   private:
      GLuint m_texture = GL_NONE;
   };

}

#endif
