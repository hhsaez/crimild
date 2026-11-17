#include <crimild/core/experimental/behaviors/composites/Parallel.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

void Parallel::initialize( void )
{
   m_activeBehaviors = getBehaviors();
   // Completion belongs to this activation, regardless of a child's old state.
   m_completed.assign( m_activeBehaviors.size(), false );
}

Behavior::State Parallel::update( void )
{
   bool allCompleted = true;
   for ( std::size_t i = 0; i < m_activeBehaviors.size(); ++i ) {
      if ( m_completed[ i ] ) {
         continue;
      }
      const auto state = m_activeBehaviors[ i ]->tick();
      if ( state == State::FAILURE || state == State::ABORTED ) {
         return state;
      }
      m_completed[ i ] = state == State::SUCCESS;
      allCompleted = allCompleted && m_completed[ i ];
   }
   return allCompleted ? State::SUCCESS : State::RUNNING;
}

void Parallel::terminate( void )
{
   Composite::terminate();
   m_activeBehaviors.clear();
   m_completed.clear();
}
