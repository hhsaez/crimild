#include <Crimild.hpp>
#include <Crimild_UniversalPlayer.hpp>

using namespace crimild;
using namespace crimild::universal;

auto main( int argc, char *argv[] ) -> int
{
   crimild::init();
   CRIMILD_LOG_INFO( crimild::Version().getFullDescription() );

   crimild::Log::setOutputHandlers(
      {
         std::make_shared< ConsoleOutputHandler >( Log::LOG_LEVEL_ALL, true ),
      }
   );

   GLFWSystem glfwSystem;
   if ( !glfwSystem.isInitialized() ) {
      return -1;
   }

   GLFWOpenGLWindow window( 1024, 768, "Triangle3D" );
   if ( !window.isValid() ) {
      return -1;
   }

   auto geometry = std::make_shared< nodes::Geometry3D >();
   geometry->setPrimitive(
      [] {
         auto primitive = std::make_shared< Primitive >( Primitive::Type::TRIANGLES );
         primitive->setVertexData(
            {
               [] {
                  return std::make_shared< VertexBuffer >(
                     VertexP3TC2::getLayout(),
                     Array< VertexP3TC2 > {
                        {
                           .position = Vector3f { -0.5f, -0.5f, 0.0f },
                           .texCoord = Vector2f { 0, 0 },
                        },
                        {
                           .position = Vector3f { 0.5f, -0.5f, 0.0f },
                           .texCoord = Vector2f { 0, 0 },
                        },
                        {
                           .position = Vector3f { 0.0f, 0.5f, 0.0f },
                           .texCoord = Vector2f { 0, 0 },
                        },
                     }
                  );
               }(),
            }
         );
         primitive->setIndices(
            std::make_shared< IndexBuffer >(
               Format::INDEX_32_UINT,
               Array< UInt32 > { 0, 1, 2 }
            )
         );
         return primitive;
      }()
   );
   geometry->setMaterial(
      [] {
         auto material = std::make_shared< UnlitMaterial >();
         material->setColorMap( Texture::ONE );
         return material;
      }()
   );

   auto camera = std::make_shared< nodes::Camera3D >();
   camera->setLocal(
      lookAt(
         Point3f { 0, 0, 2 },
         Point3f { 0, 0, 0 },
         Vector3f::Constants::UP
      )
   );

   crimild::universal::RenderPass render;

   while ( window.isOpen() ) {
      render( window.getWidth(), window.getHeight(), geometry, camera );
      window.swapBuffers();
      glfwSystem.update();
   }

   return 0;
}
