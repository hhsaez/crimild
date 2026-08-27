#ifndef CRIMILD_UNIVERSAL_SYSTEMS_WINDOW_
#define CRIMILD_UNIVERSAL_SYSTEMS_WINDOW_

#include <crimild/core/experimental/systems/System.hpp>

// Forward declaration prevents including the full GLFW headers
struct GLFWwindow;

namespace crimild::universal::systems {

   class GLFWWindow : public experimental::systems::System {
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
