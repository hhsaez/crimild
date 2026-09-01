#ifndef CRIMILD_UNIVERSAL_RENDERING_SHADER_PROGRAM_
#define CRIMILD_UNIVERSAL_RENDERING_SHADER_PROGRAM_

#include "rendering/OpenGLBindable.hpp"

#include <crimild/math/ColorRGBA.hpp>
#include <crimild/math/Matrix4.hpp>
#include <crimild/universal/foundation/GLUtils.hpp>
#include <memory>
#include <vector>

namespace crimild::universal {

   class Shader;

   class ShaderProgram : public Bindable {
   public:
      explicit ShaderProgram( std::vector< std::shared_ptr< Shader > > const &shaders ) noexcept;
      virtual ~ShaderProgram( void ) noexcept;

      virtual void bind( void ) override;
      virtual void unbind( void ) override;
      virtual void unload( void ) override;

      void setUniform( const char *name, const Matrix4f &value );
      void setUniform( const char *name, const ColorRGBA &value );

   private:
      GLuint m_id = GL_NONE;
      std::vector< std::shared_ptr< Shader > > m_shaders;
   };

}

#endif
