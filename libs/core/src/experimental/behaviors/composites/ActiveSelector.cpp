#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

Behavior::State ActiveSelector::update( void )
{
   // Snapshot the children so attachments from callbacks become visible next tick.
   const auto behaviors = getBehaviors();
   for ( const auto &behavior : behaviors ) {
      const auto state = behavior->tick();
      if ( getState() == State::ABORTED ) {
         return State::RUNNING;
      }
      if ( state == State::ABORTED ) {
         abort();
         return State::RUNNING;
      }
      if ( state != State::FAILURE ) {
         // A higher-priority child may have displaced a running child.
         for ( const auto &other : behaviors ) {
            if ( other != nullptr && other != behavior ) {
               other->abort();
               if ( getState() == State::ABORTED ) {
                  return State::RUNNING;
               }
            }
         }
         return state;
      }
   }
   return State::FAILURE;
}
