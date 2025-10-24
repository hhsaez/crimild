#include "Rendering/OpenGLIndexBufferBindable.hpp"

using namespace crimild::opengl;

IndexBufferBindable::~IndexBufferBindable( void ) noexcept
{
   if ( m_buffer != GL_NONE ) {
      unload();
   }
}

void IndexBufferBindable::bind( void )
{
   if ( m_buffer == GL_NONE ) {
      load();
   }

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffer );
}

void IndexBufferBindable::unbind( void )
{
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_NONE );
}

void IndexBufferBindable::load( void )
{
   CRIMILD_LOG_TRACE();

   auto ibo = getOwner< IndexBuffer >();
   auto view = ibo->getBufferView();

   glGenBuffers( 1, &m_buffer );
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffer );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, view->getLength(), view->getData(), GL_STATIC_DRAW );
}

void IndexBufferBindable::unload( void )
{
   CRIMILD_LOG_TRACE();

   glDeleteBuffers( 1, &m_buffer );
   m_buffer = GL_NONE;
}
