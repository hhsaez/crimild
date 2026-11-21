#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <crimild/foundation/common/Macros.hpp>
#include <crimild/foundation/log/Log.hpp>

using namespace crimild::experimental;
using namespace crimild::experimental::behaviors;

Behavior::State Behavior::tick( std::shared_ptr< Blackboard > const &blackboard )
{
   // Pin the argument across callbacks that may release the caller's handle.
   const auto board = blackboard;
   if ( board == nullptr ) {
      CRIMILD_LOG_ERROR( "Behavior tick requires a Blackboard" );
      return State::FAILURE;
   }
   if ( m_state != State::RUNNING ) {
      m_state = State::RUNNING;
      initialize( board );
   }

   if ( m_state == State::ABORTED ) {
      return m_state;
   }

   const auto result = update( board );
   // abort() has already stored the result and performed cleanup.
   if ( m_state == State::ABORTED ) {
      return m_state;
   }
   CRIMILD_INVARIANT(
      result == State::SUCCESS || result == State::FAILURE || result == State::RUNNING,
      "update() must return SUCCESS, FAILURE, or RUNNING"
   );
   m_state = result;

   if ( m_state != State::RUNNING ) {
      terminate( board );
   }

   return m_state;
}

void Behavior::abort( std::shared_ptr< Blackboard > const &blackboard )
{
   if ( m_state != State::RUNNING ) {
      return;
   }

   const auto board = blackboard;
   if ( board == nullptr ) {
      CRIMILD_LOG_ERROR( "Aborting a running behavior requires a Blackboard" );
      return;
   }

   m_state = State::ABORTED;
   terminate( board );
}

void Behavior::initialize( std::shared_ptr< Blackboard > const &blackboard )
{
   // no-op
}

Behavior::State Behavior::update( std::shared_ptr< Blackboard > const &blackboard )
{
   return State::SUCCESS;
}

void Behavior::terminate( std::shared_ptr< Blackboard > const &blackboard )
{
   // no-op
}

bool Behavior::canAttach( const Behavior *child ) const
{
   if ( child == nullptr ) {
      return false;
   }
   for ( auto ancestor = this; ancestor != nullptr; ancestor = ancestor->m_parent ) {
      if ( ancestor == child ) {
         CRIMILD_LOG_ERROR( "Behavior attachment would create a cycle" );
         return false;
      }
   }
   if ( child->m_parent == this ) {
      return false;
   }
   return true;
}

bool Behavior::canEdit( const Behavior *child, std::shared_ptr< Blackboard > const &blackboard ) const
{
   if ( blackboard != nullptr ) {
      return true;
   }
   const bool running = m_state == State::RUNNING;
   const bool runningChild = child != nullptr && child->m_state == State::RUNNING;
   const bool runningOwner = child != nullptr && child->m_parent != nullptr && child->m_parent->m_state == State::RUNNING;
   if ( running || runningChild || runningOwner ) {
      CRIMILD_LOG_ERROR( "Editing running behaviors requires a Blackboard" );
      return false;
   }
   return true;
}

std::shared_ptr< Behavior > Behavior::detachFromParent( std::shared_ptr< Blackboard > const &board )
{
   const auto blackboard = board;
   const auto self = retain( this );
   if ( auto parent = m_parent ) {
      // Retain managed owners across cleanup callbacks; construction-time and
      // stack owners still work without calling shared_from_this() on them.
      const auto retainedParent = parent->weak_from_this().lock();
      parent->detach( self, blackboard );
   }
   return self;
}

void Behavior::detach( std::shared_ptr< Behavior > const &, std::shared_ptr< Blackboard > const &blackboard )
{
   // no-op
}
