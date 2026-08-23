#include <crimild/core/experimental/behaviors/composites/Composite.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::composites;

void Composite::attach( std::shared_ptr< Behavior > const &behavior )
{
   m_behaviors.push_back( behavior );
}
