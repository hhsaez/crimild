#include <crimild/foundation/log/Log.hpp>
#include <crimild/universal/foundation/GLUtils.hpp>
#include <crimild/universal/systems/GLFWSystem.hpp>

using namespace crimild::experimental::systems;
using namespace crimild::universal::systems;

GLFWSystem::GLFWSystem( void ) noexcept
{
   CRIMILD_LOG_INFO( "Initializing GLFW System" );

   glfwSetErrorCallback(
      []( int error, const char *description ) {
         CRIMILD_LOG_ERROR( "GLFW Error (", error, "): ", description );
      }
   );

   if ( !glfwInit() ) {
      setState( State::FAILURE );
      CRIMILD_LOG_FATAL( "Cannot initialize GLFW System" );
   }

#if defined( CRIMILD_PLATFORM_WIN32 ) || defined( CRIMILD_PLATFORM_LINUX )
   // TODO: Maybe move this to a different system?
   int version = gladLoadGL( glfwGetProcAddress );
   if ( version == 0 ) {
      CRIMILD_LOG_FATAL( "Failed to initialized GLAD" );
      setState( State::FAILURE );
   } else {
      CRIMILG_LOG_INFO( "GLAD Initialized (v", version, ")" );
   }
#endif
}

GLFWSystem::~GLFWSystem( void ) noexcept
{
   detachAll();

   if ( getState() != State::FAILURE ) {
      CRIMILD_LOG_INFO( "Destroying GLFW System" );
      glfwTerminate();
   }
}

GLFWSystem::State GLFWSystem::onUpdate( const Context &ctx )
{
   glfwPollEvents();
   return System::onUpdate( ctx );
}
