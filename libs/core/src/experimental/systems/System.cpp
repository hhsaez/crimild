#include <crimild/core/experimental/systems/System.hpp>

using namespace crimild::experimental::systems;

System::~System( void ) noexcept
{
   detachAll();
}

void System::attach( std::shared_ptr< System > const &system )
{
   m_children.insert( system );
}

void System::detachAll( void )
{
   m_children.clear();
}

System::State System::update( const Context &ctx )
{
   if ( m_state == State::FAILURE ) {
      return State::FAILURE;
   }
   m_state = onUpdate( ctx );
   return m_state;
}

System::State System::onUpdate( const Context &ctx )
{
   auto ret = State::SUCCESS;

   // Get a snapshot of children collection so we can mutate
   // it during traversal.
   auto cs = getChildren();
   for ( auto s : cs ) {
      switch ( s->update( ctx ) ) {
         case State::FAILURE:
            return State::FAILURE;
         case State::RUNNING:
            ret = State::RUNNING;
            break;
         default:
            break;
      }
   }
   return ret;
}
