#include "OpenGLMaterialBindable.hpp"

#include "OpenGLShaderProgram.hpp"

using namespace crimild::opengl;

void MaterialBindable::bind( void )
{
   if ( m_program == nullptr ) {
      m_program = createProgram();
   }

   if ( auto program = m_program ) {
      program->bind();
   }
}

void MaterialBindable::unbind( void )
{
   if ( auto program = m_program ) {
      m_program->unbind();
   }
}
