#ifndef CRIMILD_UNIVERSAL_SYSTEMS_SIMULATION_
#define CRIMILD_UNIVERSAL_SYSTEMS_SIMULATION_

#include <crimild/core/experimental/systems/System.hpp>

namespace crimild::universal::systems {

   class SimulationSystem : public crimild::experimental::systems::System {
   private:
      using State = crimild::experimental::systems::System::State;
      using Context = crimild::experimental::systems::Context;

   public:
      virtual ~SimulationSystem( void ) noexcept = default;

      virtual State onUpdate( const Context &ctx ) override;
   };

}

#endif
