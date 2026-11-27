#ifndef CRIMILD_CORE_EXPERIMENTAL_SYSTEMS_TICK_SYSTEM_
#define CRIMILD_CORE_EXPERIMENTAL_SYSTEMS_TICK_SYSTEM_

#include <crimild/core/experimental/systems/System.hpp>

namespace crimild::experimental::systems {

   class TickSystem : public System {
   protected:
      virtual State onUpdate( const Context &ctx ) override;

   private:
      crimild::Clock m_clock;
   };

}

#endif
