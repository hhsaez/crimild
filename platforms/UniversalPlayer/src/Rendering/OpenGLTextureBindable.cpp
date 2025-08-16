#include "Rendering/OpenGLTextureBindable.hpp"

using namespace crimild::opengl;

TextureBindable::~TextureBindable( void )
{
   if ( m_texture != GL_NONE ) {
      unload();
   }
}

void TextureBindable::bind( void )
{
   if ( m_texture == GL_NONE ) {
      load();
   }

   glBindTexture( GL_TEXTURE_2D, m_texture );
}

void TextureBindable::unbind( void )
{
   glBindTexture( GL_TEXTURE_2D, GL_NONE );
}

void TextureBindable::load( void )
{
   CRIMILD_LOG_TRACE();

   auto texture = getOwner< Texture >();
   auto image = texture->imageView->image;
   auto width = image->getWidth();
   auto height = image->getHeight();
   auto bpp = image->getBpp();
   auto data = image->getData();

   glGenTextures( 1, &m_texture );

   glBindTexture( GL_TEXTURE_2D, m_texture );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexImage2D(
      GL_TEXTURE_2D,
      0,
      bpp == 3 ? GL_RGB : GL_RGBA,
      width,
      height,
      0,
      bpp == 3 ? GL_RGB : GL_RGBA,
      GL_UNSIGNED_BYTE,
      data
   );
   glGenerateMipmap( GL_TEXTURE_2D );
}

void TextureBindable::unload( void )
{
   CRIMILD_LOG_TRACE();

   if ( m_texture ) {
      glDeleteTextures( 1, &m_texture );
      m_texture = GL_NONE;
   }
}
