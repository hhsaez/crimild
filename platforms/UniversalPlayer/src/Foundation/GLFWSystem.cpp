#include "Foundation/GLFWSystem.hpp"

#include "Foundation/OpenGLUtils.hpp"

#include <Crimild.hpp>

using namespace crimild::universal;

GLFWSystem::GLFWSystem( void ) noexcept
{
   CRIMILD_LOG_INFO( "Initializing GLFW System" );

   glfwSetErrorCallback(
      []( int error, const char *description ) {
         CRIMILD_LOG_ERROR( "GLFW Error (", error, "): ", description );
      }
   );

   m_initialized = glfwInit();
   if ( !m_initialized ) {
      CRIMILD_LOG_FATAL( "Cannot initialize GLFW System" );
   }
}

GLFWSystem::~GLFWSystem( void ) noexcept
{
   if ( isInitialized() ) {
      CRIMILD_LOG_INFO( "Destroying GLFW System" );
      m_initialized = false;
   }
}

void GLFWSystem::update( void )
{
   glfwPollEvents();
}
