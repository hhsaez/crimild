#include "OpenGLMaterialBindable.hpp"

#include "OpenGLShader.hpp"
#include "OpenGLShaderProgram.hpp"

using namespace crimild::opengl;

MaterialBindable::~MaterialBindable( void )
{
   CRIMILD_LOG_TRACE();
   unload();
}

void MaterialBindable::bind( void )
{
   if ( m_program == nullptr ) {
      load();
   }

   m_program->bind();
}

void MaterialBindable::unbind( void )
{
   m_program->unbind();
}

void MaterialBindable::load( void )
{
   CRIMILD_LOG_TRACE();

   m_program = crimild::alloc< ShaderProgram >(
      std::vector< std::shared_ptr< Shader > > {
         crimild::alloc< Shader >(
            Shader::Stage::VERTEX,
            R"(
               #version 330 core
               #extension GL_ARB_separate_shader_objects : enable
      
               layout ( location = 0 ) in vec3 inPos;
               layout ( location = 1 ) in vec3 inColor;
      
               layout ( location = 0 ) out vec3 outColor;
      
               void main()
               {
                  gl_Position = vec4( inPos, 1.0 );
                  outColor = inColor;
               }
            )"
         ),
         crimild::alloc< Shader >(
            Shader::Stage::FRAGMENT,
            R"(
               #version 330 core
               #extension GL_ARB_separate_shader_objects : enable
      
               layout( location = 0 ) in vec3 inColor;
      
               layout( location = 0 ) out vec4 outFragColor;
      
               void main()
               {
                  outFragColor = vec4( inColor, 1.0 );
               }
            )"
         ),
      }
   );
}

void MaterialBindable::unload( void )
{
   CRIMILD_LOG_TRACE();
   m_program = nullptr;
}
