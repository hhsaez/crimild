#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>
#include <gtest/gtest.h>

namespace {

   using crimild::experimental::behaviors::Behavior;
   using State = Behavior::State;

   class TestDecorator : public crimild::experimental::behaviors::decorators::Decorator {
   public:
      TestDecorator( void ) = default;
   };

   class ControlledDecorator : public TestDecorator {
   public:
      State result = State::RUNNING;

   protected:
      State update( void ) override
      {
         if ( auto child = getBehavior() ) {
            child->tick();
         }
         return result;
      }
   };

   class TrackedBehavior : public Behavior {
   public:
      int terminations = 0;
      State terminatedWith = State::READY;

   protected:
      State update( void ) override { return State::RUNNING; }

      void terminate( void ) override
      {
         ++terminations;
         terminatedWith = getState();
      }
   };

}

TEST( experimental_Decorator, starts_without_a_child )
{
   TestDecorator decorator;
   EXPECT_EQ( decorator.getBehavior(), nullptr );
}

TEST( experimental_Decorator, attaches_child_without_executing_it )
{
   TestDecorator decorator;
   auto child = std::make_shared< Behavior >();
   decorator.attach( child );
   EXPECT_EQ( decorator.getBehavior(), child );
   EXPECT_EQ( child->getState(), Behavior::State::READY );
}

TEST( experimental_Decorator, replaces_existing_child )
{
   TestDecorator decorator;
   auto first = std::make_shared< Behavior >();
   auto second = std::make_shared< Behavior >();
   decorator.attach( first );
   decorator.attach( second );
   EXPECT_EQ( decorator.getBehavior(), second );
}

TEST( experimental_Decorator, abort_propagates_to_the_running_child_once )
{
   ControlledDecorator decorator;
   auto child = std::make_shared< TrackedBehavior >();
   decorator.attach( child );
   ASSERT_EQ( decorator.tick(), State::RUNNING );

   decorator.abort();
   decorator.abort();

   EXPECT_EQ( decorator.getState(), State::ABORTED );
   EXPECT_EQ( child->getState(), State::ABORTED );
   EXPECT_EQ( child->terminatedWith, State::ABORTED );
   EXPECT_EQ( child->terminations, 1 );
}

TEST( experimental_Decorator, abort_without_a_child_is_safe )
{
   ControlledDecorator decorator;
   ASSERT_EQ( decorator.tick(), State::RUNNING );
   decorator.abort();
   EXPECT_EQ( decorator.getState(), State::ABORTED );
}

TEST( experimental_Decorator, completion_aborts_a_child_that_is_still_running )
{
   for ( const auto result : { State::SUCCESS, State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      ControlledDecorator decorator;
      decorator.result = result;
      auto child = std::make_shared< TrackedBehavior >();
      decorator.attach( child );

      EXPECT_EQ( decorator.tick(), result );
      EXPECT_EQ( child->getState(), State::ABORTED );
      EXPECT_EQ( child->terminations, 1 );
   }
}

TEST( experimental_Decorator, replacing_a_running_child_aborts_the_previous_activation )
{
   ControlledDecorator decorator;
   auto previous = std::make_shared< TrackedBehavior >();
   auto replacement = std::make_shared< TrackedBehavior >();
   decorator.attach( previous );
   ASSERT_EQ( decorator.tick(), State::RUNNING );

   decorator.attach( replacement );

   EXPECT_EQ( previous->getState(), State::ABORTED );
   EXPECT_EQ( previous->terminations, 1 );
   EXPECT_EQ( replacement->getState(), State::READY );
   ASSERT_EQ( decorator.tick(), State::RUNNING );
   decorator.abort();
   EXPECT_EQ( replacement->getState(), State::ABORTED );
   EXPECT_EQ( replacement->terminations, 1 );
   EXPECT_EQ( previous->terminations, 1 );
}

TEST( experimental_Decorator, reattaching_the_same_child_keeps_its_activation_running )
{
   ControlledDecorator decorator;
   auto child = std::make_shared< TrackedBehavior >();
   decorator.attach( child );
   ASSERT_EQ( decorator.tick(), State::RUNNING );

   decorator.attach( child );

   EXPECT_EQ( child->getState(), State::RUNNING );
   EXPECT_EQ( child->terminations, 0 );
   decorator.abort();
   EXPECT_EQ( child->terminations, 1 );
}
