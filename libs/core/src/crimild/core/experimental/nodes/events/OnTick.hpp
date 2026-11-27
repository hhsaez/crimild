#ifndef CRIMILD_CORE_EXPERIMENTAL_NODES_EVENTS_ONTICK_
#define CRIMILD_CORE_EXPERIMENTAL_NODES_EVENTS_ONTICK_

#include <crimild/core/Common/Signal.hpp>
#include <crimild/core/Simulation/Clock.hpp>
#include <crimild/core/experimental/nodes/events/Event.hpp>

namespace crimild::experimental {

   namespace keys {

      inline constexpr Key< crimild::Clock > Clock { "crimild.clock" };

   }

   class OnTick : public Event {
   public:
      using Event::trigger;

      // Publish a Clock value before executing the behavior.
      virtual State trigger( const crimild::Clock &clock );

      // Existing signal subscribers are notified after behavior execution.
      crimild::Signal< const crimild::Clock & > triggered;

   public:
      virtual ~OnTick( void ) noexcept = default;
   };

}

#endif
