#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>

using namespace crimild::experimental;
using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

void Sequence::initialize( std::shared_ptr< Blackboard > const &blackboard )
{
   // Each activation starts with a fresh snapshot. Structural edits abort it.
   m_activeBehaviors = getBehaviors();
   m_currentBehavior = 0;
}

Behavior::State Sequence::update( std::shared_ptr< Blackboard > const &blackboard )
{
   while ( m_currentBehavior < m_activeBehaviors.size() ) {
      // Retain the child locally: cancellation can clear the activation snapshot.
      const auto child = m_activeBehaviors[ m_currentBehavior ];
      const auto state = child->tick( blackboard );
      if ( getState() == State::ABORTED ) {
         return State::RUNNING;
      }
      if ( state == State::ABORTED ) {
         abort( blackboard );
         return State::RUNNING;
      }
      if ( state != State::SUCCESS ) {
         return state;
      }
      ++m_currentBehavior;
   }
   return State::SUCCESS;
}

void Sequence::terminate( std::shared_ptr< Blackboard > const &blackboard )
{
   Composite::terminate( blackboard );
   m_activeBehaviors.clear();
   m_currentBehavior = 0;
}
