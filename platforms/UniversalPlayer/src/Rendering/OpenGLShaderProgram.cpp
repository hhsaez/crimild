#include "OpenGLShaderProgram.hpp"

#include "OpenGLShader.hpp"

#include <Crimild_Foundation.hpp>

using namespace crimild::opengl;

ShaderProgram::ShaderProgram( std::vector< std::shared_ptr< Shader > > const &shaders ) noexcept
   : m_shaders( shaders )
{
   CRIMILD_LOG_TRACE();

   m_id = glCreateProgram();
   for ( auto &shader : m_shaders ) {
      glAttachShader( m_id, shader->getId() );
   }
   glLinkProgram( m_id );
   GLint linkSuccess;
   glGetProgramiv( m_id, GL_LINK_STATUS, &linkSuccess );
   if ( linkSuccess == GL_FALSE ) {
      GLint logLength;
      glGetProgramiv( m_id, GL_INFO_LOG_LENGTH, &logLength );
      if ( logLength > 0 ) {
         std::vector< char > log( logLength );
         glGetProgramInfoLog( m_id, logLength, nullptr, log.data() );
         CRIMILD_LOG_ERROR( "Shader program link failed: ", log.data() );
      }
   }
}

ShaderProgram::~ShaderProgram( void ) noexcept
{
   CRIMILD_LOG_TRACE();

   if ( m_id != GL_NONE ) {
      glDeleteProgram( m_id );
   }
}

void ShaderProgram::bind( void ) const
{
   glUseProgram( m_id );
}

void ShaderProgram::unbind( void ) const
{
   glUseProgram( GL_NONE );
}
