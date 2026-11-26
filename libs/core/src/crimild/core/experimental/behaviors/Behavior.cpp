#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <crimild/foundation/common/Macros.hpp>
#include <crimild/foundation/log/Log.hpp>

using namespace crimild::experimental::behaviors;

Behavior::State Behavior::tick( void )
{
   if ( m_state != State::RUNNING ) {
      initialize();
      CRIMILD_INVARIANT(
         m_state == State::RUNNING || m_state == State::ABORTED,
         "initialize() must call Behavior::initialize()"
      );
      if ( m_state == State::ABORTED ) {
         return m_state;
      }
   }

   const auto result = update();
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
      terminate( m_state );
   }

   return m_state;
}

void Behavior::abort( void )
{
   if ( m_state != State::RUNNING ) {
      return;
   }

   m_state = State::ABORTED;
   terminate( State::ABORTED );
}

void Behavior::initialize( void )
{
   m_state = State::RUNNING;
}

Behavior::State Behavior::update( void )
{
   return State::SUCCESS;
}

void Behavior::terminate( State state )
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

std::shared_ptr< Behavior > Behavior::detachFromParent( void )
{
   const auto self = retain( this );
   if ( auto parent = m_parent ) {
      // Retain managed owners across cleanup callbacks; construction-time and
      // stack owners still work without calling shared_from_this() on them.
      const auto retainedParent = parent->weak_from_this().lock();
      parent->detach( self );
   }
   return self;
}

void Behavior::detach( std::shared_ptr< Behavior > const & )
{
   // no-op
}
