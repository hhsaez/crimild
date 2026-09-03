#include "systems/SimulationSystem.hpp"

#include <crimild/core/Simulation/Clock.hpp>
#include <crimild/core/experimental/nodes/Node.hpp>
#include <crimild/core/experimental/nodes/events/OnTick.hpp>
#include <crimild/core/experimental/nodes/visitors/NodeVisitor.hpp>
#include <crimild/core/experimental/nodes/visitors/Tick.hpp>
#include <crimild/foundation/log/Log.hpp>

using namespace crimild::universal::systems;
using namespace crimild::experimental::systems;

SimulationSystem::State SimulationSystem::onUpdate( const Context &ctx )
{
   static crimild::Clock clock;
   clock.tick();

   if ( auto maybeScene = ctx.get( keys::Scene ) ) {
      if ( auto scene = *maybeScene ) {
         scene->perform< experimental::Tick >( clock );
         //  broadPhase( scene );
      } else {
         CRIMILD_LOG_ERROR( "Invalid scene" );
         return State::FAILURE;
      }
   } else {
      CRIMILD_LOG_ERROR( "No scene found" );
      return State::FAILURE;
   }

   return State::SUCCESS;
}
