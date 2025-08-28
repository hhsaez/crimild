#ifndef CRIMILD_UNIVERSAL_GLFW_OPENGL_WINDOW_
#define CRIMILD_UNIVERSAL_GLFW_OPENGL_WINDOW_

#include <Crimild.hpp>
#include <string>

struct GLFWwindow;

namespace crimild::universal {

   struct KeyInfo {
      UInt32 key = CRIMILD_INPUT_KEY_UNKNOWN;
      UInt32 scancode = 0;
      UInt32 mod = 0;
   };

   class GLFWOpenGLWindow {
   public:
      GLFWOpenGLWindow( uint32_t width, uint32_t height, std::string title ) noexcept;
      virtual ~GLFWOpenGLWindow( void ) noexcept;

      Signal< int, double, double > onMouseButtonPressed;
      Signal< int, double, double > onMouseButtonReleased;
      Signal< KeyInfo > onKeyPressed;
      Signal< KeyInfo > onKeyReleased;
      Signal< KeyInfo > onKeyRepeated;

      inline bool isValid( void ) const
      {
         return m_window != nullptr;
      }

      inline uint32_t getWidth( void ) const { return m_width; }
      inline uint32_t getHeight( void ) const { return m_height; }

      void close( void );

      bool isOpen( void ) const;
      void swapBuffers( void ) const;

   private:
      GLFWwindow *m_window = nullptr;
      uint32_t m_width = 1;
      uint32_t m_height = 1;
   };
}

#endif
