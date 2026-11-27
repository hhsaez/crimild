#ifndef CRIMILD_UNIVERSAL_SYSTEMS_SIMULATION_
#define CRIMILD_UNIVERSAL_SYSTEMS_SIMULATION_

#include <crimild/core/experimental/systems/TickSystem.hpp>

namespace crimild::universal::systems {

   class [[deprecated( "Use crimild::experimental::systems::TickSystem" )]] SimulationSystem
      : public crimild::experimental::systems::TickSystem {
   public:
      using TickSystem::onUpdate;
   };

}

#endif
