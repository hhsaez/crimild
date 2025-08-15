#include "Rendering/OpenGLVertexBufferBindable.hpp"

using namespace crimild::opengl;

VertexBufferBindable::~VertexBufferBindable( void )
{
   if ( m_buffer != GL_NONE ) {
      unload();
   }
}

void VertexBufferBindable::bind( void )
{
   if ( m_buffer == GL_NONE ) {
      load();
   }
}

void VertexBufferBindable::unbind( void )
{
   // no-op
}

void VertexBufferBindable::load( void )
{
   CRIMILD_LOG_TRACE();

   auto vertices = getOwner< VertexBuffer >();
   auto view = vertices->getBufferView();

   glGenBuffers( 1, &m_buffer );
   glBindBuffer( GL_ARRAY_BUFFER, m_buffer );
   glBufferData( GL_ARRAY_BUFFER, view->getLength(), view->getData(), GL_STATIC_DRAW );

   GLuint attribIndex = 0;

   if ( auto positions = vertices->get( VertexAttribute::POSITION ) ) {
      glVertexAttribPointer(
         attribIndex,
         3,
         GL_FLOAT,
         GL_FALSE,
         positions->getBufferView()->getStride(),
         reinterpret_cast< const GLvoid * >( positions->getOffset() )
      );
      glEnableVertexAttribArray( attribIndex );
      attribIndex++;
   }

   if ( auto colors = vertices->get( VertexAttribute::COLOR ) ) {
      glVertexAttribPointer(
         attribIndex,
         3,
         GL_FLOAT,
         GL_FALSE,
         colors->getBufferView()->getStride(),
         reinterpret_cast< const GLvoid * >( colors->getOffset() )
      );
      glEnableVertexAttribArray( attribIndex );
      attribIndex++;
   }
}

void VertexBufferBindable::unload( void )
{
   CRIMILD_LOG_TRACE();
   if ( m_buffer != GL_NONE ) {
      glDeleteBuffers( 1, &m_buffer );
      m_buffer = GL_NONE;
   }
}
