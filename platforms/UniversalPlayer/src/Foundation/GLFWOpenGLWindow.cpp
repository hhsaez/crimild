#include "Foundation/GLFWOpenGLWindow.hpp"

#include "Foundation/OpenGLUtils.hpp"
#include "GLFW/glfw3.h"

using namespace crimild::universal;

GLFWOpenGLWindow::GLFWOpenGLWindow( uint32_t width, uint32_t height, std::string title ) noexcept
   : m_width( width ),
     m_height( height )
{
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   // Enable forward compatibility to remove deprecated OpenGL features and ensure
   // compatibility across platforms. This is required on macOS for OpenGL 3.3+ contexts
   // and helps prevent use of legacy OpenGL functionality. It's optional for other
   // platforms, but recommended.
   glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

   m_window = glfwCreateWindow( width, height, title.c_str(), nullptr, nullptr );
   if ( m_window == nullptr ) {
      CRIMILD_LOG_ERROR( "Failed to create OpenGL window" );
      return;
   }

   glfwSetWindowUserPointer( m_window, this );

   glfwSetFramebufferSizeCallback(
      m_window,
      []( GLFWwindow *window, int width, int height ) {
         auto windowInstance = static_cast< GLFWOpenGLWindow * >( glfwGetWindowUserPointer( window ) );
         if ( windowInstance ) {
            CRIMILD_LOG_DEBUG( "Resizing framebuffer", Vector2i { width, height } );
            windowInstance->m_width = width;
            windowInstance->m_height = height;
         }
      }
   );

   int fbw, fbh;
   glfwGetFramebufferSize( m_window, &fbw, &fbh );
   m_width = fbw;
   m_height = fbh;
   CRIMILD_LOG_INFO( "Created framebuffer ", Vector2ui { m_width, m_height } );

   glfwSetKeyCallback(
      m_window,
      []( GLFWwindow *window, int key, int scancode, int action, int mods ) {
         auto self = static_cast< GLFWOpenGLWindow * >( glfwGetWindowUserPointer( window ) );
         if ( self != nullptr && self->m_window == window ) {
            const auto info = KeyInfo {
               .key = UInt32( key ),
               .scancode = UInt32( scancode ),
               .mod = UInt32( mods ),
            };

            if ( action == GLFW_PRESS ) {
               self->onKeyPressed( info );
            } else if ( action == GLFW_RELEASE ) {
               self->onKeyReleased( info );
            } else if ( action == GLFW_REPEAT ) {
               self->onKeyRepeated( info );
            }
         }
      }
   );

   glfwSetMouseButtonCallback(
      m_window,
      []( GLFWwindow *window, int button, int action, int mods ) {
         auto self = static_cast< GLFWOpenGLWindow * >( glfwGetWindowUserPointer( window ) );
         if ( self != nullptr && self->m_window == window ) {
            double x, y;
            glfwGetCursorPos( window, &x, &y );

            if ( action == GLFW_PRESS ) {
               self->onMouseButtonPressed( button, x, y );
            } else if ( action == GLFW_RELEASE ) {
               self->onMouseButtonReleased( button, x, y );
            }
         }
      }
   );

   glfwMakeContextCurrent( m_window );

   #ifdef CRIMILD_PLATFORM_WIN32
      // TODO: Maybe move this to a different system?
      int version = gladLoadGL( glfwGetProcAddress );
      if ( version == 0 ) {
         std::cerr << "Failed to initialize GLAD" << std::endl;
         // glfwTerminate();
         exit( EXIT_FAILURE );
      }
   #endif

   glfwSwapInterval( 1 );
}

GLFWOpenGLWindow::~GLFWOpenGLWindow( void ) noexcept
{
   CRIMILD_LOG_INFO( "Destroying OpenGL Window" );
   if ( m_window != nullptr ) {
      glfwDestroyWindow( m_window );
      m_window = nullptr;
   }
}

void GLFWOpenGLWindow::resize( uint32_t width, uint32_t height )
{
   CRIMILD_LOG_DEBUG( "Resizing window: ", width, "x", height );
   glfwSetWindowSize( m_window, width, height );
}

void GLFWOpenGLWindow::close( void )
{
   if ( m_window != nullptr ) {
      glfwSetWindowShouldClose( m_window, GLFW_TRUE );
   }
}

bool GLFWOpenGLWindow::isOpen( void ) const
{
   return m_window != nullptr ? !glfwWindowShouldClose( m_window ) : false;
}

void GLFWOpenGLWindow::swapBuffers( void ) const
{
   glfwSwapBuffers( m_window );
}
