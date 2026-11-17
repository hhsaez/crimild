#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

void Sequence::initialize( void )
{
   // Freeze the child list for this activation so callbacks can safely attach
   // children without changing the remembered traversal.
   m_activeBehaviors = getBehaviors();
   m_currentBehavior = 0;
}

Behavior::State Sequence::update( void )
{
   while ( m_currentBehavior < m_activeBehaviors.size() ) {
      const auto state = m_activeBehaviors[ m_currentBehavior ]->tick();
      if ( state != State::SUCCESS ) {
         return state;
      }
      ++m_currentBehavior;
   }
   return State::SUCCESS;
}

void Sequence::terminate( void )
{
   Composite::terminate();
   m_activeBehaviors.clear();
   m_currentBehavior = 0;
}
