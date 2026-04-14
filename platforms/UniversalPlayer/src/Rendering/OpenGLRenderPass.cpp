#include "Rendering/OpenGLRenderPass.hpp"

#include "Foundation/OpenGLUtils.hpp"
#include "Rendering/OpenGLMaterialBindable.hpp"
#include "Rendering/OpenGLPrimitiveBindable.hpp"
#include "Rendering/OpenGLShaderProgram.hpp"
#include "Rendering/OpenGLUnlitMaterialBindable.hpp"

#include <Crimild.hpp>

namespace crimild::experimental {

   class RenderScene : public NodeVoidConstVisitor {
   public:
      RenderScene( std::shared_ptr< Camera3D > const &camera ) : m_camera( camera ) { }
      virtual ~RenderScene( void ) = default;

      virtual void visitGeometry3D( const Geometry3D &geometry )
      {
         if ( auto material = geometry.getMaterial() ) {
            if ( auto bindable = getMaterialBindable( material ) ) {
               bindable->bind();

               auto program = bindable->getProgram();
               program->setUniform( "uProjMatrix", m_camera->getProjectionMatrix() );
               program->setUniform( "uViewMatrix", m_camera->getViewMatrix() );
               program->setUniform( "uModelMatrix", crimild::Matrix4f( geometry.getWorld() ) );

               if ( auto primitive = geometry.getPrimitive() ) {
                  auto bindable = primitive->template getOrCreateExtension< opengl::PrimitiveBindable >();
                  bindable->bind();
                  bindable->render();
                  bindable->unbind();
               }

               bindable->unbind();
            }
         }
      }

   private:
      std::shared_ptr< crimild::opengl::MaterialBindable > getMaterialBindable( std::shared_ptr< Material > const &material ) const
      {
         if ( auto bindable = material->getExtension< crimild::opengl::MaterialBindable >() ) {
            return bindable;
         }

         if ( auto unlit = std::dynamic_pointer_cast< crimild::UnlitMaterial >( material ) ) {
            auto bindable = std::make_shared< crimild::opengl::UnlitMaterialBindable >();
            material->attach( crimild::opengl::MaterialBindable::__CLASS_NAME, bindable );
            return bindable;
         }

         return nullptr;
      }

   private:
      std::shared_ptr< Camera3D > m_camera;
   };

}

using namespace crimild::universal;

void RenderPass::operator()(
   uint32_t width,
   uint32_t height,
   std::shared_ptr< experimental::Node > const &node,
   std::shared_ptr< experimental::Camera3D > const &camera,
   ColorRGBA clearColor
) const
{
   glViewport( 0, 0, width, height );
   glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
   glClear( GL_COLOR_BUFFER_BIT );

   camera->setProjectionMatrix( perspective( 60.0f, ( float ) width / ( float ) height, 0.1f, 1000.0f ) );

   //   render( node, camera );

   node->perform< crimild::experimental::RenderScene >( camera );

   auto error = glGetError();
   if ( error != GL_NO_ERROR ) {
      CRIMILD_LOG_ERROR( "GL Error (", ( int ) error, ")" );
   }
}

/*
void RenderPass::render(
   std::shared_ptr< nodes::Node > const &node,
   std::shared_ptr< nodes::Camera3D > const &camera
) const
{
   if ( auto geometry = dynamic_pointer_cast< nodes::Geometry3D >( node ) ) {
      if ( auto material = geometry->getMaterial() ) {
         if ( auto bindable = getMaterialBindable( material ) ) {
            bindable->bind();

            auto program = bindable->getProgram();
            program->setUniform( "uProjMatrix", camera->getProjectionMatrix() );
            program->setUniform( "uViewMatrix", camera->getViewMatrix() );
            program->setUniform( "uModelMatrix", crimild::Matrix4f( geometry->getWorld() ) );

            if ( auto primitive = geometry->getPrimitive() ) {
               auto bindable = primitive->template getOrCreateExtension< opengl::PrimitiveBindable >();
               bindable->bind();
               bindable->render();
               bindable->unbind();
            }

            bindable->unbind();
         }
      }
   }

   for ( auto child : node->getChildren() ) {
      render( child, camera );
   }
}

std::shared_ptr< crimild::opengl::MaterialBindable > RenderPass::getMaterialBindable( std::shared_ptr< Material > const &material ) const
{
   if ( auto bindable = material->getExtension< crimild::opengl::MaterialBindable >() ) {
      return bindable;
   }

   if ( auto unlit = std::dynamic_pointer_cast< crimild::UnlitMaterial >( material ) ) {
      auto bindable = std::make_shared< crimild::opengl::UnlitMaterialBindable >();
      material->attach( crimild::opengl::MaterialBindable::__CLASS_NAME, bindable );
      return bindable;
   }

   return nullptr;
}
*/
