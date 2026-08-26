#ifndef CRIMILD_UNIVERSAL_FOUNDATION_GL_UTILS_
#define CRIMILD_UNIVERSAL_FOUNDATION_GL_UTILS_

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
