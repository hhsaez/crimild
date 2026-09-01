#include "rendering/OpenGLMaterialBindable.hpp"

#include "rendering/OpenGLShaderProgram.hpp"

#include <crimild/foundation/log/Log.hpp>

using namespace crimild::universal;

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

void MaterialBindable::unload( void )
{
   CRIMILD_LOG_TRACE();

   if ( auto program = m_program ) {
      program->unload();
   }
}
