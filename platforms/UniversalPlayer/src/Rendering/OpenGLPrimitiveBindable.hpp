#ifndef CRIMILD_RENDERING_BINDABLE_PRIMITIVE_
#define CRIMILD_RENDERING_BINDABLE_PRIMITIVE_

#include "Foundation/OpenGLUtils.hpp"
#include "Rendering/OpenGLBindable.hpp"

namespace crimild::opengl {

   class PrimitiveBindable : public crimild::opengl::Bindable {
      CRIMILD_IMPLEMENT_RTTI( crimild::opengl::PrimitiveBindable )

   public:
      virtual ~PrimitiveBindable( void );

      virtual void bind( void ) override;

      void render( void );

      virtual void unbind( void ) override;

   private:
      void load( void );
      void unload( void );

   private:
      bool m_loaded = false;
      GLuint m_vao;
      // GLuint m_positionsVBO;
      // GLuint m_colorsVBO;
      // GLuint m_indicesEBO;
   };

}

#endif
