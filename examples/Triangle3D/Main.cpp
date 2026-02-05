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

   while ( window.isOpen() ) {
      window.swapBuffers();
      glfwSystem.update();
   }

   return 0;
}
