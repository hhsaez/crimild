#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>

using namespace crimild::experimental;
using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::decorators;

Decorator::~Decorator( void ) noexcept
{
   if ( m_behavior != nullptr ) {
      m_behavior->setParent( nullptr );
   }
}

void Decorator::attach( std::shared_ptr< Behavior > const &behavior, std::shared_ptr< Blackboard > const &board )
{
   const auto blackboard = board;
   const auto child = behavior;
   if ( m_behavior == child || ( child != nullptr && !canAttach( child.get() ) ) ) {
      return;
   }
   if ( !canEdit( child.get(), blackboard ) || !canEdit( m_behavior.get(), blackboard ) ) {
      return;
   }
   abort( blackboard );
   if ( child != nullptr && !canAttach( child.get() ) ) {
      return;
   }
   if ( child != nullptr ) {
      child->detachFromParent( blackboard );
      if ( !canAttach( child.get() ) || child->hasParent() ) {
         return;
      }
   }
   if ( const auto previous = m_behavior ) {
      detach( previous, blackboard );
   }
   // A child cleanup callback may have installed a replacement of its own.
   if ( m_behavior != nullptr || ( child != nullptr && ( !canAttach( child.get() ) || child->hasParent() ) ) ) {
      return;
   }
   m_behavior = child;
   if ( child != nullptr ) {
      child->setParent( this );
   }
}

void Decorator::detach( std::shared_ptr< Behavior > const &behavior, std::shared_ptr< Blackboard > const &board )
{
   const auto blackboard = board;
   const auto child = behavior;
   if ( child == nullptr || child != m_behavior ) {
      return;
   }
   if ( !canEdit( child.get(), blackboard ) ) {
      return;
   }
   abort( blackboard );
   if ( child != m_behavior ) {
      return;
   }
   child->abort( blackboard );
   if ( child == m_behavior ) {
      m_behavior.reset();
      child->setParent( nullptr );
   }
}

void Decorator::terminate( std::shared_ptr< Blackboard > const &blackboard )
{
   if ( auto behavior = getBehavior() ) {
      behavior->abort( blackboard );
   }
}
