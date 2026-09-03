#ifndef CRIMILD_UNIVERSAL_SYSTEMS_WINDOW_
#define CRIMILD_UNIVERSAL_SYSTEMS_WINDOW_

#include <crimild/core/Common/Signal.hpp>
#include <crimild/core/Simulation/Input.hpp>
#include <crimild/core/experimental/systems/System.hpp>
#include <cstdint>

// Forward declaration prevents including the full GLFW headers
struct GLFWwindow;

namespace crimild::universal::systems {

   struct KeyInfo {
      uint32_t key = CRIMILD_INPUT_KEY_UNKNOWN;
      uint32_t scancode = 0;
      uint32_t mod = 0;
   };

   class GLFWWindow : public experimental::systems::System {
   public:
      Signal< int, double, double > onMouseButtonPressed;
      Signal< int, double, double > onMouseButtonReleased;
      Signal< KeyInfo > onKeyPressed;
      Signal< KeyInfo > onKeyReleased;
      Signal< KeyInfo > onKeyRepeated;

   public:
      virtual ~GLFWWindow( void ) noexcept;

   protected:
      virtual State onUpdate( const experimental::systems::Context &ctx ) override;

   private:
      bool setup( const experimental::systems::Context &ctx );

   private:
      GLFWwindow *m_window = nullptr;
      Size2i m_size;
   };

}

#endif
