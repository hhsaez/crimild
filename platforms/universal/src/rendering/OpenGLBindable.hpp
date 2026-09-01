#ifndef CRIMILD_UNIVERSAL_RENDERING_BINDABLE_
#define CRIMILD_UNIVERSAL_RENDERING_BINDABLE_

#include <crimild/core/Entity/Extension.hpp>

namespace crimild::universal {

   class Bindable : public crimild::Extension {
   protected:
      Bindable( void ) = default;

   public:
      virtual ~Bindable( void ) = default;

      virtual void bind( void ) = 0;
      virtual void unbind( void ) = 0;

      virtual void unload( void ) = 0;
   };

}

#endif
