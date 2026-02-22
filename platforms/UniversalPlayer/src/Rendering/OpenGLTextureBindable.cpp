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
   auto width = image->extent.width;
   auto height = image->extent.height;
   auto format = image->format == Format::R8G8B8_UNORM ? GL_RGB : GL_RGBA;
   auto data = image->getBufferView()->getData();

   glGenTextures( 1, &m_texture );

   glBindTexture( GL_TEXTURE_2D, m_texture );

   auto wrapMode = GL_REPEAT;
   auto minFilter = GL_NEAREST;
   auto magFilter = GL_NEAREST;
   if ( auto sampler = texture->sampler ) {
      wrapMode = [ sampler ] {
         switch ( sampler->getWrapMode() ) {
            case Sampler::WrapMode::REPEAT:
               return GL_REPEAT;
            case Sampler::WrapMode::CLAMP_TO_EDGE:
               return GL_CLAMP_TO_EDGE;
            default:
               break;
         }
      }();

      minFilter = [ sampler ] {
         switch ( sampler->getMinFilter() ) {
            case Sampler::Filter::NEAREST:
               return GL_NEAREST;
            case Sampler::Filter::LINEAR:
               return GL_LINEAR;
            default:
               break;
         }
      }();

      magFilter = [ sampler ] {
         switch ( sampler->getMagFilter() ) {
            case Sampler::Filter::NEAREST:
               return GL_NEAREST;
            case Sampler::Filter::LINEAR:
               return GL_LINEAR;
            default:
               break;
         }
      }();
   }
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );

   glTexImage2D(
      GL_TEXTURE_2D,
      0,
      format,
      width,
      height,
      0,
      format,
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
