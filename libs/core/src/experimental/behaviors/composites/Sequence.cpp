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
      const auto state = b->tick();
      if ( state != State::SUCCESS ) {
         // An earlier child can take control while a later child is running.
         for ( const auto &other : behaviors ) {
            if ( other != nullptr && other != b ) {
               other->abort();
            }
         }
         return state;
      }
   }
   return State::SUCCESS;
}
