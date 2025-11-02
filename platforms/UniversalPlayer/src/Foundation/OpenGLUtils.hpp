#ifndef CRIMILD_OPENGL_UTILS_
#define CRIMILD_OPENGL_UTILS_

#include <Crimild_Foundation.hpp>

// clang-format off
#ifdef CRIMILD_PLATFORM_DESKTOP
   #ifdef CRIMILD_PLATFORM_OSX
      // GLFW_INCLUDE_GLCOREARB makes the GLFW header include the modern GL/glcorearb.h header (OpenGL/gl3.h on macOS) 
      // instead of the regular OpenGL header.
      #define GLFW_INCLUDE_GLCOREARB
      #include <GLFW/glfw3.h>
   #else 
      #include <glad/gl.h>
      #include <GLFW/glfw3.h>
   #endif
#endif
// clang-format on

#endif
