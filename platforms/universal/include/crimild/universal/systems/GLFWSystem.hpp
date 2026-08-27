#ifndef CRIMILD_UNIVERSAL_SYSTEMS_GLFW_
#define CRIMILD_UNIVERSAL_SYSTEMS_GLFW_

#include <crimild/core/experimental/systems/System.hpp>

namespace crimild::universal::systems {

   class GLFWSystem : public experimental::systems::System {
   public:
      GLFWSystem( void ) noexcept;
      virtual ~GLFWSystem( void ) noexcept;

   protected:
      virtual State onUpdate( const experimental::systems::Context &ctx ) override;
   };

}

#endif
