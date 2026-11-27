#include <crimild/core/experimental/nodes/events/OnTick.hpp>

using namespace crimild::experimental;

OnTick::State OnTick::trigger( const crimild::Clock &clock )
{
   if ( !isEnabled() ) {
      return State::SUCCESS;
   }
   const auto self = weak_from_this().lock();
   if ( const auto blackboard = getBlackboard() ) {
      blackboard->set( keys::Clock, clock );
   }
   const auto result = Event::trigger();
   triggered( clock );
   return result;
}
