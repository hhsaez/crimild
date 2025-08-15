#ifndef CRIMILD_OPENGL_RENDERING_BINDABLE_INDEX_BUFFER_
#define CRIMILD_OPENGL_RENDERING_BINDABLE_INDEX_BUFFER_

#include "Foundation/OpenGLUtils.hpp"
#include "Rendering/OpenGLBindable.hpp"

namespace crimild::opengl {

   class IndexBufferBindable : public Bindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::opengl::IndexBufferBindable )

   public:
      virtual ~IndexBufferBindable( void ) noexcept;

      virtual void bind( void ) override;
      virtual void unbind( void ) override;

   private:
      void load( void );
      void unload( void );

   private:
      GLuint m_buffer = GL_NONE;
   };

}

#endif
