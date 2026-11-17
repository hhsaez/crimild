#include <crimild/core/experimental/behaviors/composites/Selector.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

void Selector::initialize( void )
{
   // Freeze the child list for this activation so callbacks can safely attach
   // children without changing the remembered traversal.
   m_activeBehaviors = getBehaviors();
   m_currentBehavior = 0;
}

Behavior::State Selector::update( void )
{
   while ( m_currentBehavior < m_activeBehaviors.size() ) {
      const auto state = m_activeBehaviors[ m_currentBehavior ]->tick();
      if ( state != State::FAILURE ) {
         return state;
      }
      ++m_currentBehavior;
   }
   return State::FAILURE;
}

void Selector::terminate( void )
{
   Composite::terminate();
   m_activeBehaviors.clear();
   m_currentBehavior = 0;
}
