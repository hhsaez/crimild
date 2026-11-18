#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::decorators;

void Decorator::attach( std::shared_ptr< Behavior > const &behavior )
{
   if ( m_behavior == behavior ) {
      return;
   }

   const auto previous = m_behavior;
   m_behavior = behavior;
   if ( previous != nullptr ) {
      previous->abort();
   }
}

void Decorator::terminate( void )
{
   if ( auto behavior = getBehavior() ) {
      behavior->abort();
   }
}
