#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>
#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>
#include <functional>
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
      std::function< void() > onUpdate;

   protected:
      State update( void ) override
      {
         if ( onUpdate ) {
            onUpdate();
         }
         return State::RUNNING;
      }

      void terminate( State state ) override
      {
         ++terminations;
         terminatedWith = state;
         EXPECT_EQ( state, getState() );
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

TEST( experimental_Decorator, attachment_automatically_reparents_and_prevents_cycles )
{
   auto parent = std::make_shared< TestDecorator >();
   auto nested = std::make_shared< TestDecorator >();
   auto destination = std::make_shared< TestDecorator >();
   parent->attach( nested );
   parent->attach( parent );
   nested->attach( parent );
   EXPECT_FALSE( parent->hasParent() );
   EXPECT_EQ( parent->getBehavior(), nested );
   EXPECT_EQ( nested->getBehavior(), nullptr );
   destination->attach( nested );
   EXPECT_EQ( parent->getBehavior(), nullptr );
   EXPECT_EQ( nested->getParent(), destination );
   destination->attach( nullptr );
   EXPECT_FALSE( nested->hasParent() );
}

TEST( experimental_Decorator, replacement_and_destruction_clear_parent_pointers )
{
   auto previous = std::make_shared< Behavior >();
   auto child = std::make_shared< Behavior >();
   {
      auto decorator = std::make_shared< TestDecorator >();
      decorator->attach( previous );
      decorator->attach( child );
      EXPECT_FALSE( previous->hasParent() );
      EXPECT_EQ( child->getParent(), decorator );
   }
   EXPECT_FALSE( child->hasParent() );
}

TEST( experimental_Decorator, replacement_during_first_and_later_ticks_preserves_abort )
{
   for ( const bool firstTick : { true, false } ) {
      auto decorator = std::make_shared< ControlledDecorator >();
      auto child = std::make_shared< TrackedBehavior >();
      auto replacement = std::make_shared< TrackedBehavior >();
      decorator->attach( child );
      if ( !firstTick ) {
         ASSERT_EQ( decorator->tick(), State::RUNNING );
      }
      child->onUpdate = [ & ] {
         decorator->attach( replacement );
      };
      EXPECT_EQ( decorator->tick(), State::ABORTED );
      EXPECT_EQ( child->terminations, 1 );
      EXPECT_FALSE( child->hasParent() );
      EXPECT_EQ( replacement->getParent(), decorator );
      EXPECT_EQ( replacement->getState(), State::READY );
      EXPECT_EQ( decorator->tick(), State::RUNNING );
      decorator->abort();
      EXPECT_EQ( replacement->terminations, 1 );
   }
}

TEST( experimental_Decorator, reparenting_between_composite_and_decorator_cancels_active_work )
{
   auto sequence = std::make_shared< crimild::experimental::behaviors::composites::Sequence >();
   auto decorator = std::make_shared< ControlledDecorator >();
   auto child = std::make_shared< TrackedBehavior >();
   auto foreign = std::make_shared< TrackedBehavior >();
   decorator->attach( child );
   sequence->attach( foreign );
   ASSERT_EQ( decorator->tick(), State::RUNNING );
   ASSERT_EQ( sequence->tick(), State::RUNNING );
   sequence->attach( child );
   EXPECT_EQ( decorator->getState(), State::ABORTED );
   EXPECT_EQ( sequence->getState(), State::ABORTED );
   EXPECT_EQ( decorator->getBehavior(), nullptr );
   EXPECT_EQ( child->getParent(), sequence );
   EXPECT_EQ( child->terminations, 1 );
   EXPECT_EQ( foreign->terminations, 1 );
   decorator->attach( foreign );
   EXPECT_EQ( decorator->getBehavior(), foreign );
   EXPECT_EQ( foreign->getParent(), decorator );
   ASSERT_EQ( sequence->getBehaviors().size(), 1 );
   EXPECT_EQ( sequence->getBehaviors().front(), child );
   EXPECT_EQ( decorator->tick(), State::RUNNING );
   decorator->abort();
   EXPECT_EQ( foreign->terminations, 2 );
}
