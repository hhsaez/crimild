#include <crimild/core/experimental/behaviors/Behavior.hpp>

using namespace crimild::experimental::behaviors;

Behavior::State Behavior::tick( void )
{
   m_state = update();
   return m_state;
}

Behavior::State Behavior::update( void )
{
   return State::SUCCESS;
}
