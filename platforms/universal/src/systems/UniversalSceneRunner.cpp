#include "systems/GLFWInputSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/SimulationSystem.hpp"

#include <crimild/universal/systems/GLFWSystem.hpp>
#include <crimild/universal/systems/GLFWWindow.hpp>
#include <crimild/universal/systems/UniversalSceneRunner.hpp>
#include <memory>

using namespace crimild::universal::systems;

SceneRunner::SceneRunner( int argc, char **argv ) noexcept
   : experimental::systems::Runner( argc, argv )
{
   attach(
      [] {
         auto glfw = std::make_shared< GLFWSystem >();
         glfw->attach(
            [] {
               auto window = std::make_shared< GLFWWindow >();
               window->attach< GLFWInputSystem >();
               // TODO: simulation should be window independent
               window->attach< SimulationSystem >();
               window->attach< RenderSystem >();
               return window;
            }()
         );
         return glfw;
      }()
   );
}
