#include <crimild/core/experimental/nodes/Node.hpp>
#include <crimild/core/experimental/nodes/visitors/Tick.hpp>
#include <crimild/core/experimental/systems/TickSystem.hpp>
#include <crimild/foundation/log/Log.hpp>

using namespace crimild::experimental::systems;

System::State TickSystem::onUpdate( const Context &ctx )
{
   const auto scene = ctx.get( keys::Scene );
   if ( !scene || *scene == nullptr ) {
      CRIMILD_LOG_ERROR( "Cannot tick a missing or invalid scene" );
      return State::FAILURE;
   }

   m_clock.tick();
   ( *scene )->perform< crimild::experimental::Tick >( m_clock );
   // Behavior FAILURE is a decision result, not a System failure.
   return System::onUpdate( ctx );
}
