#include <crimild/core/experimental/behaviors/Behavior.hpp>

using namespace crimild::experimental::behaviors;

Behavior::State Behavior::tick( void )
{
   if ( m_state != State::RUNNING ) {
      initialize();
   }

   m_state = update();

   if ( m_state != State::RUNNING ) {
      terminate( m_state );
   }

   return m_state;
}

void Behavior::initialize( void )
{
}

Behavior::State Behavior::update( void )
{
   return State::SUCCESS;
}

void Behavior::terminate( State )
{
}
