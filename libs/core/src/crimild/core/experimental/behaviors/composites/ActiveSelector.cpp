#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>

using namespace crimild::experimental;
using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

Behavior::State ActiveSelector::update( Blackboard &blackboard )
{
   // Keep children alive if a callback edits the collection and aborts us.
   const auto behaviors = getBehaviors();
   for ( const auto &behavior : behaviors ) {
      const auto state = behavior->tick( blackboard );
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
