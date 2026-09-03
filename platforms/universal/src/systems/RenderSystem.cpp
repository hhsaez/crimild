#include "systems/RenderSystem.hpp"

#include "rendering/OpenGLMaterialBindable.hpp"
#include "rendering/OpenGLPrimitiveBindable.hpp"
#include "rendering/OpenGLShaderProgram.hpp"
#include "rendering/OpenGLUnlitMaterialBindable.hpp"

#include <crimild/core/Primitives/Primitive.hpp>
#include <crimild/core/Rendering/Material.hpp>
#include <crimild/core/Rendering/Materials/UnlitMaterial.hpp>
#include <crimild/core/experimental/nodes/3d/Camera3D.hpp>
#include <crimild/core/experimental/nodes/3d/Geometry3D.hpp>
#include <crimild/core/experimental/nodes/visitors/NodeConstVisitor.hpp>
#include <crimild/core/experimental/systems/System.hpp>
#include <crimild/foundation/log/Log.hpp>
#include <crimild/math/ColorRGBA.hpp>
#include <crimild/math/Matrix4.hpp>
#include <crimild/math/Rect.hpp>
#include <crimild/math/ortho.hpp>
#include <crimild/universal/systems/DebugSystem.hpp>
#include <memory>

namespace crimild::experimental {

   class RenderScene : public NodeVoidConstVisitor {
   public:
      RenderScene( const crimild::universal::systems::RenderSystem &renderSystem, std::shared_ptr< Camera3D > const &camera )
         : m_renderSystem( renderSystem ),
           m_camera( camera )
      {
      }

      virtual ~RenderScene( void ) = default;

      virtual void visitGeometry3D( const Geometry3D &geometry )
      {
         if ( !geometry.isEnabled() ) {
            return;
         }

         if ( auto material = geometry.getMaterial() ) {
            if ( auto bindable = getMaterialBindable( material ) ) {
               m_renderSystem.bind( bindable );
               bindable->bind();

               auto program = bindable->getProgram();
               program->setUniform( "uProjMatrix", m_camera->getProjectionMatrix() );
               program->setUniform( "uViewMatrix", m_camera->getViewMatrix() );
               program->setUniform( "uModelMatrix", crimild::Matrix4f( geometry.getWorld() ) );

               if ( auto primitive = geometry.getPrimitive() ) {
                  auto bindable = primitive->template getOrCreateExtension< universal::PrimitiveBindable >();
                  m_renderSystem.bind( bindable );
                  bindable->bind();
                  bindable->render();
                  bindable->unbind();
               }

               bindable->unbind();
            }
         }

         NodeVoidConstVisitor::visitGeometry3D( geometry );
      }

   private:
      std::shared_ptr< crimild::universal::MaterialBindable > getMaterialBindable( std::shared_ptr< Material > const &material ) const
      {
         if ( auto bindable = material->getExtension< crimild::universal::MaterialBindable >() ) {
            return bindable;
         }

         if ( auto unlit = std::dynamic_pointer_cast< crimild::UnlitMaterial >( material ) ) {
            auto bindable = std::make_shared< crimild::universal::UnlitMaterialBindable >();
            material->attach( crimild::universal::MaterialBindable::__CLASS_NAME, bindable );
            return bindable;
         }

         return nullptr;
      }

   private:
      const crimild::universal::systems::RenderSystem &m_renderSystem;
      std::shared_ptr< Camera3D > m_camera;
   };

}

using namespace crimild::universal::systems;
using namespace crimild::experimental::systems;

RenderSystem::~RenderSystem( void ) noexcept
{
   CRIMILD_LOG_TRACE();

   for ( auto maybeResource : m_resources ) {
      if ( auto resource = maybeResource.lock() ) {
         resource->unload();
      }
   }
   m_resources.clear();
}

RenderSystem::State RenderSystem::onUpdate( const Context &ctx )
{
   if ( !ctx.contains( keys::WindowSize ) ) {
      CRIMILD_LOG_ERROR( "No window size" );
      return State::FAILURE;
   }

   if ( !ctx.contains( keys::Scene ) ) {
      CRIMILD_LOG_ERROR( "No scene to render" );
      return State::FAILURE;
   }

   if ( !ctx.contains( keys::Camera ) ) {
      CRIMILD_LOG_ERROR( "No camera to render with" );
      return State::FAILURE;
   }

   const auto targetSize = ctx.get( keys::FramebufferSize ).value();
   const auto targetAspect = float( targetSize.width ) / float( targetSize.height );

   const auto renderSize = ctx.get( keys::RenderSize ).value_or( targetSize );
   const float renderAspect = float( renderSize.width ) / float( renderSize.height );

   float l, r, t, b;
   if ( targetAspect >= renderAspect ) {
      float scale = targetSize.height / renderSize.height;
      r = renderSize.width * scale;
      b = renderSize.height * scale;
      l = 0.5f * ( targetSize.width - r );
      t = 0.0f;
   } else {
      float scale = targetSize.width / renderSize.width;
      r = renderSize.width * scale;
      b = renderSize.height * scale;
      l = 0.0f;
      t = 0.5f * ( targetSize.height - b );
   }

   auto camera = *ctx.get( keys::Camera );
   auto scene = *ctx.get( keys::Scene );

   camera->setProjectionMatrix(
      ortho(
         0,
         renderSize.width,
         0,
         renderSize.height,
         -1.0f,
         1000.0f
      )
   );
   const auto viewport = Rectf {
      .origin = {
         l,
         t,
      },
      .size = {
         .width = r,
         .height = b,
      },
   };

   render(
      viewport,
      scene,
      camera,
      0.05 * crimild::ColorRGBA { 1.0f, 0.0f, 1.0f, 0.0f }
   );

   if ( auto debugSystem = DebugSystem::getInstance() ) {
      if ( debugSystem->isEnabled() ) {
         if ( auto scene = debugSystem->getScene() ) {
            render( scene, camera );
         }
      }
   }

   return State::SUCCESS;
}

void RenderSystem::render(
   Rect viewport,
   std::shared_ptr< crimild::experimental::Node > const &node,
   std::shared_ptr< crimild::experimental::Camera3D > const &camera,
   crimild::ColorRGBA clearColor
) const
{
   // TODO: Move this to a general setup function since we only need to enable it once.
   glEnable( GL_SCISSOR_TEST );

   glViewport( viewport.origin.x, viewport.origin.y, viewport.size.width, viewport.size.height );
   glScissor( viewport.origin.x, viewport.origin.y, viewport.size.width, viewport.size.height );
   glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   node->perform< crimild::experimental::RenderScene >( *this, camera );

   auto error = glGetError();
   if ( error != GL_NO_ERROR ) {
      CRIMILD_LOG_ERROR( "GL Error (", ( int ) error, ")" );
   }
}

void RenderSystem::render(
   std::shared_ptr< crimild::experimental::Node > const &node,
   std::shared_ptr< crimild::experimental::Camera3D > const &camera
) const
{
   node->perform< crimild::experimental::RenderScene >( *this, camera );

   auto error = glGetError();
   if ( error != GL_NO_ERROR ) {
      CRIMILD_LOG_ERROR( "GL Error (", ( int ) error, ")" );
   }
}
