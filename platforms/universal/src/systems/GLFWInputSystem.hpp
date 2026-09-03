#ifndef CRIMILD_UNIVERSAL_SYSTEMS_GLFW_INPUT_
#define CRIMILD_UNIVERSAL_SYSTEMS_GLFW_INPUT_

#include <crimild/core/Simulation/Input.hpp>
#include <crimild/core/experimental/systems/System.hpp>

namespace crimild::universal::systems {

   class GLFWInputSystem : public crimild::experimental::systems::System {
   public:
      virtual ~GLFWInputSystem( void ) noexcept = default;

      virtual State onUpdate( const crimild::experimental::systems::Context &ctx ) override;

   private:
      crimild::Input m_input;
   };

}

#endif
