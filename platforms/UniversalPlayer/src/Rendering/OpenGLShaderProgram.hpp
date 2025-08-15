#ifndef CRIMILD_OPENGL_RENDERING_SHADER_PROGRAM_
#define CRIMILD_OPENGL_RENDERING_SHADER_PROGRAM_

#include "Foundation/OpenGLUtils.hpp"

#include <memory>
#include <vector>

namespace crimild::opengl {

   class Shader;

   class ShaderProgram {
   public:
      explicit ShaderProgram( std::vector< std::shared_ptr< Shader > > const &shaders ) noexcept;
      virtual ~ShaderProgram( void ) noexcept;

      void bind( void ) const;
      void unbind( void ) const;

   private:
      GLuint m_id = GL_NONE;
      std::vector< std::shared_ptr< Shader > > m_shaders;
   };

}

#endif
