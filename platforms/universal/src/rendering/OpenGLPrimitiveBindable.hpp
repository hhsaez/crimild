#ifndef CRIMILD_RENDERING_BINDABLE_PRIMITIVE_
#define CRIMILD_RENDERING_BINDABLE_PRIMITIVE_

#include "rendering/OpenGLBindable.hpp"

#include <crimild/universal/foundation/GLUtils.hpp>

namespace crimild::universal {

   class PrimitiveBindable : public Bindable {
      CRIMILD_IMPLEMENT_RTTI( PrimitiveBindable )

   public:
      virtual ~PrimitiveBindable( void );

      virtual void bind( void ) override;

      void render( void );

      virtual void unbind( void ) override;

      virtual void unload( void ) override;

   private:
      void load( void );

   private:
      bool m_loaded = false;
      GLuint m_vao;
   };

}

#endif
