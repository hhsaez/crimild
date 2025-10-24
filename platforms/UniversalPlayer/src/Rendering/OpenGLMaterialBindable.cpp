#include "OpenGLMaterialBindable.hpp"

#include "OpenGLShader.hpp"
#include "OpenGLShaderProgram.hpp"
#include "OpenGLTextureBindable.hpp"

using namespace crimild::opengl;

MaterialBindable::~MaterialBindable( void )
{
   CRIMILD_LOG_TRACE();
   unload();
}

void MaterialBindable::bind( void )
{
   auto material = getOwner< Material >();

   if ( m_program == nullptr ) {
      load();
   }

   m_program->bind();

   if ( auto texture = material->getColorMap() ) {
      texture->getOrCreateExtension< TextureBindable >()->bind();
   }
}

void MaterialBindable::unbind( void )
{
   auto material = getOwner< Material >();

   m_program->unbind();

   if ( auto texture = material->getColorMap() ) {
      texture->getOrCreateExtension< TextureBindable >()->unbind();
   }
}

void MaterialBindable::load( void )
{
   CRIMILD_LOG_TRACE();

   m_program = crimild::alloc< ShaderProgram >(
      std::vector< std::shared_ptr< Shader > > {
         crimild::alloc< Shader >(
            Shader::Stage::VERTEX,
            R"(
               #version 330 core
               #extension GL_ARB_separate_shader_objects : enable
      
               layout ( location = 0 ) in vec3 inPos;
               layout ( location = 1 ) in vec2 inTexCoord;

               uniform mat4 uProjMatrix;
               uniform mat4 uViewMatrix;
               uniform mat4 uModelMatrix;
      
               layout ( location = 0 ) out vec2 outTexCoord;
      
               void main()
               {
                  gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4( inPos, 1.0 );
                  outTexCoord = inTexCoord;
               }
            )"
         ),
         crimild::alloc< Shader >(
            Shader::Stage::FRAGMENT,
            R"(
               #version 330 core
               #extension GL_ARB_separate_shader_objects : enable
      
               layout( location = 0 ) in vec2 inTexCoord;

               uniform sampler2D uColorMap;
      
               layout( location = 0 ) out vec4 outFragColor;
      
               void main()
               {
                  outFragColor = texture( uColorMap, inTexCoord );
                  outFragColor.a = 1.0;
               }
            )"
         ),
      }
   );
}

void MaterialBindable::unload( void )
{
   CRIMILD_LOG_TRACE();
   m_program = nullptr;
}
