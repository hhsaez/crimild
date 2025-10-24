#include "OpenGLShader.hpp"

#include <Crimild_Foundation.hpp>

using namespace crimild::opengl;

Shader::Shader( Stage stage, std::string src ) noexcept
{
   CRIMILD_LOG_TRACE();

   m_id = glCreateShader( GLuint( stage ) );
   const char *srcPtr = src.c_str();
   glShaderSource( m_id, 1, &srcPtr, nullptr );
   glCompileShader( m_id );
   GLint compileStatus;
   glGetShaderiv( m_id, GL_COMPILE_STATUS, &compileStatus );
   if ( compileStatus == GL_FALSE ) {
      GLint logLength;
      glGetShaderiv( m_id, GL_INFO_LOG_LENGTH, &logLength );
      if ( logLength > 0 ) {
         std::vector< char > log( logLength );
         glGetShaderInfoLog( m_id, logLength, nullptr, log.data() );
         CRIMILD_LOG_ERROR( "Shader compilation failed (", GLuint( stage ), "): ", log.data() );
      }
   }
}

Shader::~Shader( void ) noexcept
{
   CRIMILD_LOG_TRACE();

   if ( m_id != GL_NONE ) {
      glDeleteShader( m_id );
      m_id = GL_NONE;
   }
}
