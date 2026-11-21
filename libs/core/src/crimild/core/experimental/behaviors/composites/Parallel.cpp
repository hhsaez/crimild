#include <crimild/core/experimental/behaviors/composites/Parallel.hpp>

using namespace crimild::experimental;
using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

void Parallel::initialize( std::shared_ptr< Blackboard > const &blackboard )
{
   m_activeBehaviors = getBehaviors();
   // Completion belongs to this activation, regardless of a child's old state.
   m_completed.assign( m_activeBehaviors.size(), false );
}

Behavior::State Parallel::update( std::shared_ptr< Blackboard > const &blackboard )
{
   bool allCompleted = true;
   for ( std::size_t i = 0; i < m_activeBehaviors.size(); ++i ) {
      if ( m_completed[ i ] ) {
         continue;
      }
      // Retain the child locally: cancellation can clear the activation snapshot.
      const auto child = m_activeBehaviors[ i ];
      const auto state = child->tick( blackboard );
      if ( getState() == State::ABORTED ) {
         return State::RUNNING;
      }
      if ( state == State::ABORTED ) {
         abort( blackboard );
         return State::RUNNING;
      }
      if ( state == State::FAILURE ) {
         return state;
      }
      m_completed[ i ] = state == State::SUCCESS;
      allCompleted = allCompleted && m_completed[ i ];
   }
   return allCompleted ? State::SUCCESS : State::RUNNING;
}

void Parallel::terminate( std::shared_ptr< Blackboard > const &blackboard )
{
   Composite::terminate( blackboard );
   m_activeBehaviors.clear();
   m_completed.clear();
}
