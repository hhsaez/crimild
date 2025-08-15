#include "OpenGLSceneRenderPass.hpp"

#include "Foundation/OpenGLUtils.hpp"
#include "Rendering/OpenGLMaterialBindable.hpp"
#include "Rendering/OpenGLPrimitiveBindable.hpp"

#include <Crimild.hpp>

using namespace crimild::universal;

void SceneRenderPass::operator()(
   uint32_t width,
   uint32_t height,
   std::shared_ptr< crimild::Node > const &scene,
   std::shared_ptr< crimild::Camera > const &camera
)
{
   glViewport( 0, 0, width, height );
   glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT );

   auto geometries = scene->perform< crimild::FetchGeometries >();
   for ( auto &geometry : geometries ) {
      auto ms = geometry->getComponent< crimild::MaterialComponent >();
      if ( auto material = ms->first() ) {
         auto bindable = material->getOrCreateExtension< opengl::MaterialBindable >();
         bindable->bind();

         geometry->forEachPrimitive(
            [ & ]( auto primitive ) {
               auto bindable = primitive->getOrCreateExtension< opengl::PrimitiveBindable >();
               bindable->bind();
               bindable->render();
               bindable->unbind();
            }
         );

         bindable->unbind();
      }
   }

   auto error = glGetError();
   if ( error != GL_NO_ERROR ) {
      CRIMILD_LOG_ERROR( "GL Error (", ( int ) error, ")" );
   }
}
