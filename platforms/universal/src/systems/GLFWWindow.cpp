#include <crimild/foundation.hpp>
#include <crimild/math/Vector2.hpp>
#include <crimild/math/io.hpp>
#include <crimild/universal/foundation/GLUtils.hpp>
#include <crimild/universal/systems/GLFWWindow.hpp>

using namespace crimild::universal::systems;

GLFWWindow::~GLFWWindow( void ) noexcept
{
   CRIMILD_LOG_INFO( "Destroying GLFW Window" );
   if ( m_window != nullptr ) {
      glfwDestroyWindow( m_window );
      m_window = nullptr;
   }
}

GLFWWindow::State GLFWWindow::onUpdate( const experimental::systems::Context &ctx )
{
   if ( !setup( ctx ) ) {
      return State::FAILURE;
   }

   if ( glfwWindowShouldClose( m_window ) ) {
      return State::SUCCESS;
   }

   glfwMakeContextCurrent( m_window );

   if ( System::onUpdate( ctx ) == State::FAILURE ) {
      return State::FAILURE;
   }

   glfwSwapInterval( 1 );
   glfwSwapBuffers( m_window );

   return State::RUNNING;
}

bool GLFWWindow::setup( const experimental::systems::Context &ctx )
{
   if ( m_window != nullptr ) {
      // already initialized
      return true;
   }

   if ( auto size = ctx.get( experimental::systems::keys::WindowSize ) ) {
      m_size = *size;
   }

   CRIMILD_LOG_INFO( "Creating window (", m_size.width, "x", m_size.height, ")" );

   std::string title = "Crimild";

   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
   glfwWindowHint( GLFW_RESIZABLE, GL_TRUE );

   // Enable forward compatibility to remove deprecated OpenGL features and ensure
   // compatibility across platforms. This is required on macOS for OpenGL 3.3+ contexts
   // and helps prevent use of legacy OpenGL functionality. It's optional for other
   // platforms, but recommended.
   glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

   m_window = glfwCreateWindow( m_size.width, m_size.height, title.c_str(), nullptr, nullptr );
   if ( m_window == nullptr ) {
      CRIMILD_LOG_ERROR( "Failed to create GLFW window" );
      return false;
   }

   glfwSetWindowUserPointer( m_window, this );

   // Validate window's dimensions after creation
   // It is possible the OS resizes the window to appropriated dimensions during creation
   // For example, the window dimension could be too big for the screen.
   int actualWidth, actualHeight;
   glfwGetWindowSize( m_window, &actualWidth, &actualHeight );
   m_size = { .width = actualWidth, .height = actualHeight };

   glfwSetWindowSizeCallback(
      m_window,
      []( GLFWwindow *window, int width, int height ) {
         auto windowInstance = static_cast< GLFWWindow * >( glfwGetWindowUserPointer( window ) );
         if ( windowInstance ) {
            CRIMILD_LOG_DEBUG( "Window resized", Vector2i { width, height } );
            windowInstance->m_size = { .width = width, .height = height };
         }
      }
   );

   glfwSetFramebufferSizeCallback(
      m_window,
      []( GLFWwindow *window, int width, int height ) {
         auto windowInstance = static_cast< GLFWWindow * >( glfwGetWindowUserPointer( window ) );
         if ( windowInstance ) {
            CRIMILD_LOG_DEBUG( "Resizing framebuffer", Vector2i { width, height } );
            // windowInstance->m_width = width;
            // windowInstance->m_height = height;
         }
      }
   );

   int fbw, fbh;
   glfwGetFramebufferSize( m_window, &fbw, &fbh );
   CRIMILD_LOG_INFO( "Created framebuffer ", Vector2i { fbw, fbh } );

   return true;
}
