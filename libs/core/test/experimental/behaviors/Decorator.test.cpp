#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>
#include <gtest/gtest.h>

namespace {
   using crimild::experimental::behaviors::Behavior;
   class TestDecorator : public crimild::experimental::behaviors::decorators::Decorator {
   public:
      TestDecorator( void ) = default;
   };
}

TEST( ExperimentalDecorator, starts_without_a_child )
{
   TestDecorator decorator;
   EXPECT_EQ( decorator.getBehavior(), nullptr );
}

TEST( ExperimentalDecorator, attaches_child_without_executing_it )
{
   TestDecorator decorator;
   auto child = std::make_shared< Behavior >();
   decorator.attach( child );
   EXPECT_EQ( decorator.getBehavior(), child );
   EXPECT_EQ( child->getState(), Behavior::State::READY );
}

TEST( ExperimentalDecorator, replaces_existing_child )
{
   TestDecorator decorator;
   auto first = std::make_shared< Behavior >();
   auto second = std::make_shared< Behavior >();
   decorator.attach( first );
   decorator.attach( second );
   EXPECT_EQ( decorator.getBehavior(), second );
}
