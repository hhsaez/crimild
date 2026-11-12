#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>

using namespace crimild::experimental::behaviors;
using namespace crimild::experimental::behaviors::decorators;

void Decorator::attach( std::shared_ptr< Behavior > const &behavior )
{
   m_behavior = behavior;
}
