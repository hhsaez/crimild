#include <crimild/core/experimental/systems/System.hpp>

using namespace crimild::experimental::systems;

System::~System( void ) noexcept
{
   detachAll();
}

std::shared_ptr< System > System::detachFromParent( void )
{
   // Since we're detaching from our owner, keep a retained pointer so we're
   // not destroyed yet.
   auto self = retain( this );
   if ( auto parent = getParent() ) {
      parent->detach( self );
   }
   return self;
}

void System::setParent( System *parent )
{
   m_parent = parent;
}

void System::attach( std::shared_ptr< System > const &child )
{
   if ( child == nullptr ) {
      return;
   }

   if ( child->getParent().get() == this ) {
      return;
   }

   child->detachFromParent();
   m_children.insert( child );
   child->setParent( this );
}

void System::detach( std::shared_ptr< System > const &child )
{
   if ( child == nullptr ) {
      return;
   }

   if ( child->getParent().get() != this ) {
      return;
   }

   auto it = std::find( m_children.begin(), m_children.end(), child );
   m_children.erase( it );
   child->setParent( nullptr );
}

void System::detachAll( void )
{
   for ( auto c : m_children ) {
      if ( c != nullptr ) {
         c->setParent( nullptr );
      }
   }
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
