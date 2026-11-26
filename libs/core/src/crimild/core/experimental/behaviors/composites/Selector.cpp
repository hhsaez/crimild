#include <crimild/core/experimental/behaviors/composites/Selector.hpp>

using namespace crimild::experimental;
using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

void Selector::initialize( Blackboard &blackboard )
{
   Composite::initialize( blackboard );
   // Each activation starts with a fresh snapshot. Structural edits abort it.
   m_activeBehaviors = getBehaviors();
   m_currentBehavior = 0;
}

Behavior::State Selector::update( Blackboard &blackboard )
{
   while ( m_currentBehavior < m_activeBehaviors.size() ) {
      // Retain the child locally: cancellation can clear the activation snapshot.
      const auto child = m_activeBehaviors[ m_currentBehavior ];
      const auto state = child->tick( blackboard );
      if ( getState() == State::ABORTED ) {
         return State::RUNNING;
      }
      if ( state == State::ABORTED ) {
         abort();
         return State::RUNNING;
      }
      if ( state != State::FAILURE ) {
         return state;
      }
      ++m_currentBehavior;
   }
   return State::FAILURE;
}

void Selector::terminate( State state )
{
   Composite::terminate( state );
   m_activeBehaviors.clear();
   m_currentBehavior = 0;
}
