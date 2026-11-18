#ifndef CRIMILD_CORE_EXPERIMENTAL_NODES_EVENTS_ONTICK_
#define CRIMILD_CORE_EXPERIMENTAL_NODES_EVENTS_ONTICK_

#include <crimild/core/Common/Signal.hpp>
#include <crimild/core/Simulation/Clock.hpp>
#include <crimild/core/experimental/nodes/events/Event.hpp>

namespace crimild::experimental {

   class OnTick : public Event {
   public:
      crimild::Signal< const crimild::Clock & > triggered;

   public:
      virtual ~OnTick( void ) noexcept = default;
   };

}

#endif
