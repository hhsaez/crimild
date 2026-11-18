#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <crimild/foundation/common/Macros.hpp>

using namespace crimild::experimental::behaviors;

Behavior::State Behavior::tick( void )
{
   if ( m_state != State::RUNNING ) {
      m_state = State::RUNNING;
      initialize();
   }

   if ( m_state == State::ABORTED ) {
      return m_state;
   }

   const auto result = update();
   // abort() has already stored the result and performed cleanup.
   if ( m_state == State::ABORTED ) {
      return m_state;
   }
   CRIMILD_INVARIANT(
      result == State::SUCCESS || result == State::FAILURE || result == State::RUNNING,
      "update() must return SUCCESS, FAILURE, or RUNNING"
   );
   m_state = result;

   if ( m_state != State::RUNNING ) {
      terminate();
   }

   return m_state;
}

void Behavior::abort( void )
{
   if ( m_state != State::RUNNING ) {
      return;
   }

   m_state = State::ABORTED;
   terminate();
}

void Behavior::initialize( void )
{
   // no-op
}

Behavior::State Behavior::update( void )
{
   return State::SUCCESS;
}

void Behavior::terminate( void )
{
   // no-op
}
