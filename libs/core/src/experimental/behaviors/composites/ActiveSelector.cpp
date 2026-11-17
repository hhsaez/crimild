#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

Behavior::State ActiveSelector::update( void )
{
   // Attachments from callbacks become visible on the next tick.
   const auto behaviors = getBehaviors();
   for ( const auto &behavior : behaviors ) {
      const auto state = behavior->tick();
      if ( state != State::FAILURE ) {
         // A higher-priority child may have displaced a running child.
         for ( const auto &other : behaviors ) {
            if ( other != nullptr && other != behavior ) {
               other->abort();
            }
         }
         return state;
      }
   }
   return State::FAILURE;
}
