#include "OpenGLPrimitiveBindable.hpp"

#include "OpenGLIndexBufferBindable.hpp"
#include "OpenGLVertexBufferBindable.hpp"

#include <crimild/core/Primitives/Primitive.hpp>

using namespace crimild::universal;

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
   static std::unordered_map< crimild::Primitive::Type, GLenum > glPrimitiveTypes {
      { Primitive::Type::TRIANGLES, GL_TRIANGLES },
      { Primitive::Type::LINES, GL_LINES },
      { Primitive::Type::LINE_STRIP, GL_LINE_STRIP },
      { Primitive::Type::LINE_LOOP, GL_LINE_LOOP },
   };

   auto primitive = getOwner< Primitive >();
   auto indices = primitive->getIndices();

   glDrawElements(
      glPrimitiveTypes[ primitive->getType() ],
      indices->getIndexCount(),
      GL_UNSIGNED_INT,
      0
   );
}

void PrimitiveBindable::unbind( void )
{
   glBindVertexArray( 0 );
}

void PrimitiveBindable::load( void )
{
   CRIMILD_LOG_TRACE();

   glGenVertexArrays( 1, &m_vao );
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
            if ( auto bindable = vertices->template getExtension< VertexBufferBindable >() ) {
               bindable->unload();
               vertices->detach( bindable );
            }
         }
      );
      if ( auto indices = primitive->getIndices()->getExtension< IndexBufferBindable >() ) {
         indices->unload();
         primitive->getIndices()->detach( indices );
      }
   }

   m_loaded = false;
}
