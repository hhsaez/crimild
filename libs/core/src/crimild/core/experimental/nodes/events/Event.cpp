#include <crimild/core/experimental/nodes/events/Event.hpp>
#include <crimild/foundation/log/Log.hpp>

using namespace crimild::experimental;

void Event::setBehavior( std::shared_ptr< behaviors::Behavior > const &behavior )
{
   const auto next = behavior;
   if ( next == m_behavior ) {
      return;
   }
   const auto self = weak_from_this().lock();
   const auto previous = m_behavior;
   const auto board = m_blackboard;
   if ( previous != nullptr ) {
      previous->abort();
   }
   // Preserve a configuration chosen by a cleanup callback.
   if ( m_behavior != previous || m_blackboard != board || ( previous != nullptr && previous->getState() == State::RUNNING ) ) {
      return;
   }
   m_behavior = next;
}

void Event::setBlackboard( std::shared_ptr< Blackboard > const &blackboard )
{
   const auto next = blackboard;
   if ( next == m_blackboard ) {
      return;
   }
   const auto self = weak_from_this().lock();
   const auto previous = m_blackboard;
   const auto behavior = m_behavior;
   if ( behavior != nullptr ) {
      behavior->abort();
   }
   if ( m_blackboard != previous || m_behavior != behavior || ( behavior != nullptr && behavior->getState() == State::RUNNING ) ) {
      return;
   }
   m_blackboard = next;
}

Event::State Event::trigger( void )
{
   if ( !isEnabled() ) {
      return State::SUCCESS;
   }
   // Pin both arguments at the root boundary, including across replacement callbacks.
   const auto behavior = m_behavior;
   const auto blackboard = m_blackboard;
   if ( behavior == nullptr ) {
      return State::SUCCESS;
   }
   if ( blackboard == nullptr ) {
      CRIMILD_LOG_ERROR( "Cannot trigger an event behavior without a Blackboard" );
      return State::FAILURE;
   }
   return behavior->tick( *blackboard );
}
