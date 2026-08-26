#ifndef CRIMILD_UNIVERSAL_SYSTEMS_WINDOW_
#define CRIMILD_UNIVERSAL_SYSTEMS_WINDOW_

#include <crimild/core/experimental/systems/System.hpp>

// Forward declaration prevents including the full GLFW headers
struct GLFWwindow;

namespace crimild::universal::systems {

   class GLFWWindow : public crimild::experimental::systems::System {
   public:
      virtual ~GLFWWindow( void ) noexcept;

   protected:
      virtual State onUpdate( void ) override;

   private:
      bool setup( void );

   private:
      GLFWwindow *m_window = nullptr;
      uint32_t m_width = 1024;
      uint32_t m_height = 768;
   };

}

#endif
