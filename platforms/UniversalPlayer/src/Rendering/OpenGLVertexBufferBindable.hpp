#ifndef CRIMILD_OPENGL_RENDERING_BINDABLE_VERTEX_BUFFER_
#define CRIMILD_OPENGL_RENDERING_BINDABLE_VERTEX_BUFFER_

#include "Foundation/OpenGLUtils.hpp"
#include "Rendering/OpenGLBindable.hpp"

namespace crimild::opengl {

   class VertexBufferBindable : public Bindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::opengl::VertexBufferBindable )

   public:
      virtual ~VertexBufferBindable( void ) noexcept;

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
