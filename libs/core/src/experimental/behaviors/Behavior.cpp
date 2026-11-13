#include <crimild/core/experimental/behaviors/Behavior.hpp>

using namespace crimild::experimental::behaviors;

Behavior::State Behavior::tick( void )
{
   if ( m_state != State::RUNNING ) {
      initialize();
   }

   m_state = update();

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
