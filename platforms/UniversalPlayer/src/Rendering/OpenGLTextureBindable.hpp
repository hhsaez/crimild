#ifndef CRIMILD_OPENGL_RENDERING_BINDABLE_TEXTURE_
#define CRIMILD_OPENGL_RENDERING_BINDABLE_TEXTURE_

#include "Foundation/OpenGLUtils.hpp"
#include "Rendering/OpenGLBindable.hpp"

namespace crimild::opengl {

   class TextureBindable : public Bindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::opengl::TextureBindable )

   public:
      virtual ~TextureBindable( void ) noexcept;

      virtual void bind( void ) override;
      virtual void unbind( void ) override;

   private:
      void load( void );
      void unload( void );

   private:
      GLuint m_texture = GL_NONE;
   };

}

#endif
