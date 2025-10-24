#ifndef CRIMILD_OPENGL_RENDERING_SHADER_
#define CRIMILD_OPENGL_RENDERING_SHADER_

#include "Foundation/OpenGLUtils.hpp"

#include <string>

namespace crimild::opengl {

   class Shader {
   public:
      enum class Stage : GLuint {
         VERTEX = GL_VERTEX_SHADER,
         FRAGMENT = GL_FRAGMENT_SHADER,
      };

      Shader( Stage stage, std::string src ) noexcept;
      virtual ~Shader( void ) noexcept;

      inline GLuint getId( void ) const { return m_id; }

   private:
      GLuint m_id = GL_NONE;
   };

}

#endif
