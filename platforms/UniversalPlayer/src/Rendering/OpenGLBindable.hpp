#ifndef CRIMILD_OPENGL_RENDERING_BINDABLE_
#define CRIMILD_OPENGL_RENDERING_BINDABLE_

#include <Crimild.hpp>

namespace crimild::opengl {

   class Bindable : public crimild::Extension {
   protected:
      Bindable( void ) = default;

   public:
      virtual ~Bindable( void ) = default;

      virtual void bind( void ) = 0;
      virtual void unbind( void ) = 0;
   };

}

#endif
