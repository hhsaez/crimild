#include "OpenGLPrimitiveBindable.hpp"

#include "OpenGLIndexBufferBindable.hpp"
#include "OpenGLVertexBufferBindable.hpp"

using namespace crimild::opengl;

PrimitiveBindable::~PrimitiveBindable( void )
{
   if ( m_loaded ) {
      unload();
   }
}

void PrimitiveBindable::bind( void )
{
   if ( !m_loaded ) {
      load();
   }

   glBindVertexArray( m_vao );
}

void PrimitiveBindable::render( void )
{
   auto primitive = getOwner< Primitive >();
   auto indices = primitive->getIndices();

   glDrawElements( GL_TRIANGLES, indices->getIndexCount(), GL_UNSIGNED_INT, 0 );
}

void PrimitiveBindable::unbind( void )
{
   glBindVertexArray( GL_NONE );
}

void PrimitiveBindable::load( void )
{
   CRIMILD_LOG_TRACE();

   glCreateVertexArrays( 1, &m_vao );
   glBindVertexArray( m_vao );

   auto primitive = getOwner< Primitive >();

   primitive->getVertexData().first()->getOrCreateExtension< VertexBufferBindable >()->bind();
   primitive->getIndices()->getOrCreateExtension< IndexBufferBindable >()->bind();

   m_loaded = true;
}

void PrimitiveBindable::unload( void )
{
   CRIMILD_LOG_TRACE();

   glDeleteVertexArrays( 1, &m_vao );

   if ( auto primitive = getOwner< Primitive >() ) {
      primitive->getVertexData().each(
         []( auto vertices ) {
            vertices->detach< VertexBufferBindable >();
         }
      );
      primitive->getIndices()->detach< IndexBufferBindable >();
   }

   m_loaded = false;
}
