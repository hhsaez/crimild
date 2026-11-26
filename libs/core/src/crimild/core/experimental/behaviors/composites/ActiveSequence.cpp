#include <crimild/core/experimental/behaviors/composites/ActiveSequence.hpp>
#include <cstddef>

using namespace crimild::experimental;
using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

Behavior::State ActiveSequence::update( Blackboard &blackboard )
{
   // Retain children across callbacks that edit the collection and abort us.
   const auto behaviors = getBehaviors();
   for ( std::size_t i = 0; i < behaviors.size(); ++i ) {
      const auto state = behaviors[ i ]->tick( blackboard );
      if ( getState() == State::ABORTED ) {
         return State::RUNNING;
      }
      if ( state == State::ABORTED ) {
         abort();
         return State::RUNNING;
      }
      if ( state != State::SUCCESS ) {
         // An earlier child may have blocked previously running work.
         for ( std::size_t j = i + 1; j < behaviors.size(); ++j ) {
            behaviors[ j ]->abort();
            if ( getState() == State::ABORTED ) {
               return State::RUNNING;
            }
         }
         return state;
      }
   }
   return State::SUCCESS;
}
