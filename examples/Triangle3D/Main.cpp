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
                           .texCoord = Vector2f { 0, 1 },
                        },
                        {
                           .position = Vector3f { 0.5f, -0.5f, 0.0f },
                           .texCoord = Vector2f { 1, 1 },
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
         //         material->setColorMap( Texture::ONE );
         material->setColorMap(
            [] {
               uint8_t colors[] = {
                  // red
                  0xff,
                  0x00,
                  0x00,
                  0xff,
                  // black
                  0x00,
                  0x00,
                  0x00,
                  0xff,
                  // green
                  0x00,
                  0xff,
                  0x00,
                  0xff,
                  // blue
                  0x00,
                  0x00,
                  0xff,
                  0xff,
               };
               crimild::Int32 width = 2;
               crimild::Int32 height = 2;
               crimild::Int32 bpp = 4;

               auto data = ByteArray( width * height * bpp );
               for ( int y = 0; y < width; y++ ) {
                  for ( int x = 0; x < width; x++ ) {
                     auto colorIdx = ( y % 2 + x % 2 ) % 2;
                     for ( int i = 0; i < bpp; i++ ) {
                        data[ y * width * bpp + x * bpp + i ] = colors[ colorIdx * bpp + i ];
                     }
                  }
               }

               auto image = crimild::alloc< Image >();
               image->extent = {
                  .width = 2,
                  .height = 2,
                  .depth = 1,
               };
               image->format = Format::R8G8B8A8_UNORM;
               image->setBufferView(
                  std::make_shared< BufferView >(
                     BufferView::Target::IMAGE,
                     std::make_shared< Buffer >( data )
                  )
               );
               auto texture = crimild::alloc< Texture >();
               texture->imageView = std::make_shared< ImageView >();
               texture->imageView->image = image;
               texture->sampler = [ & ] {
                  auto sampler = crimild::alloc< Sampler >();
                  sampler->setMinFilter( Sampler::Filter::LINEAR );
                  sampler->setMagFilter( Sampler::Filter::LINEAR );
                  return sampler;
               }();
               return texture;
            }()
         );
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
