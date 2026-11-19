#include <crimild/core/experimental/behaviors/composites/Composite.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

void Composite::attach( std::shared_ptr< Behavior > const &behavior )
{
   m_behaviors.push_back( behavior );
}

void Composite::terminate( void )
{
   // Keep callbacks that modify the collection from invalidating traversal.
   const auto behaviors = getBehaviors();
   for ( const auto &behavior : behaviors ) {
      if ( behavior != nullptr ) {
         behavior->abort();
      }
   }
}
