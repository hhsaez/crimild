#ifndef CRIMILD_UNIVERSAL_RENDERING_BINDABLE_INDEX_BUFFER_
#define CRIMILD_UNIVERSAL_RENDERING_BINDABLE_INDEX_BUFFER_

#include "rendering/OpenGLBindable.hpp"

#include <crimild/universal/foundation/GLUtils.hpp>

namespace crimild::universal {

   class IndexBufferBindable : public Bindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::universal::IndexBufferBindable )

   public:
      virtual ~IndexBufferBindable( void ) noexcept;

      virtual void bind( void ) override;
      virtual void unbind( void ) override;

      virtual void unload( void ) override;

   private:
      void load( void );

   private:
      GLuint m_buffer = GL_NONE;
   };

}

#endif
