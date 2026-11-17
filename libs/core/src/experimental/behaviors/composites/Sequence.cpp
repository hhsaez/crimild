#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

Behavior::State Sequence::update( void )
{
   // Reevaluate from the first child on every execution, stopping as soon
   // as a child fails or needs more time.
   // Snapshot the children so attachments during execution take effect
   // on the next traversal without invalidating this one.
   const auto behaviors = getBehaviors();
   for ( const auto &b : behaviors ) {
      switch ( b->tick() ) {
         case State::FAILURE:
            return State::FAILURE;
         case State::RUNNING:
            return State::RUNNING;
         default:
            break;
      }
   }
   return State::SUCCESS;
}
