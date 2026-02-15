#include "OpenGLUnlitMaterialBindable.hpp"

#include "OpenGLShader.hpp"
#include "OpenGLShaderProgram.hpp"
#include "OpenGLTextureBindable.hpp"

using namespace crimild::opengl;

void UnlitMaterialBindable::bind( void )
{
   MaterialBindable::bind();

   auto material = getOwner< UnlitMaterial >();

   // m_program->setUniform( "uColor", material->getColor() );

   auto texture = retain( material->getColorMap() );
   if ( texture == nullptr ) {
      texture = Texture::ONE;
   }
   texture->getOrCreateExtension< TextureBindable >()->bind();
}

void UnlitMaterialBindable::unbind( void )
{
   MaterialBindable::unbind();

   auto material = getOwner< UnlitMaterial >();

   auto texture = retain( material->getColorMap() );
   if ( texture == nullptr ) {
      texture = Texture::ONE;
   }
   texture->getOrCreateExtension< TextureBindable >()->unbind();
}

std::shared_ptr< ShaderProgram > UnlitMaterialBindable::createProgram( void )
{
   CRIMILD_LOG_TRACE();

   return std::make_shared< ShaderProgram >(
      std::vector< std::shared_ptr< Shader > > {
         std::make_shared< Shader >(
            Shader::Stage::VERTEX,
            R"(
               #version 330 core
      
               layout ( location = 0 ) in vec3 inPos;
               layout ( location = 1 ) in vec2 inTexCoord;

               uniform mat4 uProjMatrix;
               uniform mat4 uViewMatrix;
               uniform mat4 uModelMatrix;
      
               out vec2 vTexCoord;
      
               void main()
               {
                  gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4( inPos, 1.0 );
                  vTexCoord = inTexCoord;
               }
            )"
         ),
         std::make_shared< Shader >(
            Shader::Stage::FRAGMENT,
            R"(
               #version 330 core
      
               in vec2 vTexCoord;

               uniform sampler2D uColorMap;
      
               layout( location = 0 ) out vec4 outFragColor;
      
               void main()
               {
                  outFragColor = texture( uColorMap, vTexCoord );
                  outFragColor.a = 1.0;
               }
            )"
         ),
      }
   );
}
