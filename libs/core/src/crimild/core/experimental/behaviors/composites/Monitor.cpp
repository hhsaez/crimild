#include <crimild/core/experimental/behaviors/composites/Monitor.hpp>
#include <crimild/foundation/log/Log.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

Behavior::State Monitor::update( void )
{
   // Retain both branches across callbacks that edit the collection and abort us.
   const auto behaviors = getBehaviors();
   if ( behaviors.size() != 2 ) {
      CRIMILD_LOG_ERROR( "Monitor requires a condition subtree and an action subtree" );
      return State::FAILURE;
   }

   const auto conditionState = behaviors[ 0 ]->tick();
   if ( getState() == State::ABORTED ) {
      return State::RUNNING;
   }
   if ( conditionState == State::ABORTED ) {
      abort();
      return State::RUNNING;
   }
   if ( conditionState == State::FAILURE ) {
      return State::FAILURE;
   }

   const auto actionState = behaviors[ 1 ]->tick();
   if ( getState() == State::ABORTED ) {
      return State::RUNNING;
   }
   if ( actionState == State::ABORTED ) {
      abort();
      return State::RUNNING;
   }
   return actionState;
}
