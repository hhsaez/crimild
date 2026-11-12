#include <crimild/core/experimental/behaviors/Behavior.hpp>

using namespace crimild::experimental::behaviors;

Behavior::State Behavior::execute( void )
{
   m_state = onExecute();
   return m_state;
}

Behavior::State Behavior::onExecute( void )
{
   return State::SUCCESS;
}
