#include <crimild/core/experimental/behaviors/composites/Composite.hpp>
#include <functional>
#include <gtest/gtest.h>

namespace {

   using crimild::experimental::behaviors::Behavior;
   using State = Behavior::State;

   class TestComposite : public crimild::experimental::behaviors::composites::Composite {
   public:
      TestComposite( void ) = default;
   };

   class ControlledComposite : public TestComposite {
   public:
      State result = State::RUNNING;

   protected:
      State update( void ) override { return result; }
   };

   class TrackedBehavior : public Behavior {
   public:
      State result = State::RUNNING;
      State terminatedWith = State::READY;
      int terminations = 0;
      std::function< void() > onTerminate;

   protected:
      State update( void ) override { return result; }

      void terminate( void ) override
      {
         ++terminations;
         terminatedWith = getState();
         if ( onTerminate ) {
            onTerminate();
         }
      }
   };

}

TEST( experimental_Composite, starts_without_children )
{
   TestComposite composite;
   EXPECT_TRUE( composite.getBehaviors().empty() );
}

TEST( experimental_Composite, attaches_children_in_order_without_executing_them )
{
   TestComposite composite;
   auto first = std::make_shared< Behavior >();
   auto second = std::make_shared< Behavior >();
   composite.attach( first );
   composite.attach( second );

   const auto &children = composite.getBehaviors();
   ASSERT_EQ( children.size(), 2 );
   EXPECT_EQ( children[ 0 ], first );
   EXPECT_EQ( children[ 1 ], second );
   EXPECT_EQ( first->getState(), Behavior::State::READY );
   EXPECT_EQ( second->getState(), Behavior::State::READY );
}

TEST( experimental_Composite, abort_propagates_only_to_running_children )
{
   ControlledComposite composite;
   auto running = std::make_shared< TrackedBehavior >();
   auto completed = std::make_shared< TrackedBehavior >();
   auto waiting = std::make_shared< TrackedBehavior >();
   completed->result = State::SUCCESS;
   ASSERT_EQ( running->tick(), State::RUNNING );
   ASSERT_EQ( completed->tick(), State::SUCCESS );
   composite.attach( running );
   composite.attach( completed );
   composite.attach( waiting );
   ASSERT_EQ( composite.tick(), State::RUNNING );

   composite.abort();
   composite.abort();

   EXPECT_EQ( composite.getState(), State::ABORTED );
   EXPECT_EQ( running->getState(), State::ABORTED );
   EXPECT_EQ( running->terminatedWith, State::ABORTED );
   EXPECT_EQ( running->terminations, 1 );
   EXPECT_EQ( completed->getState(), State::SUCCESS );
   EXPECT_EQ( completed->terminations, 1 );
   EXPECT_EQ( waiting->getState(), State::READY );
   EXPECT_EQ( waiting->terminations, 0 );
}

TEST( experimental_Composite, completion_aborts_children_that_are_still_running )
{
   for ( const auto result : { State::SUCCESS, State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      ControlledComposite composite;
      composite.result = result;
      auto child = std::make_shared< TrackedBehavior >();
      ASSERT_EQ( child->tick(), State::RUNNING );
      composite.attach( child );

      EXPECT_EQ( composite.tick(), result );
      EXPECT_EQ( child->getState(), State::ABORTED );
      EXPECT_EQ( child->terminations, 1 );
   }
}

TEST( experimental_Composite, termination_keeps_a_snapshot_when_callbacks_attach_children )
{
   ControlledComposite composite;
   auto first = std::make_shared< TrackedBehavior >();
   auto second = std::make_shared< TrackedBehavior >();
   first->onTerminate = [ & ] {
      const auto capacity = composite.getBehaviors().capacity();
      while ( composite.getBehaviors().size() <= capacity ) {
         composite.attach( std::make_shared< Behavior >() );
      }
   };
   composite.attach( first );
   composite.attach( second );
   ASSERT_EQ( first->tick(), State::RUNNING );
   ASSERT_EQ( second->tick(), State::RUNNING );
   ASSERT_EQ( composite.tick(), State::RUNNING );

   composite.abort();

   EXPECT_EQ( first->terminations, 1 );
   EXPECT_EQ( second->getState(), State::ABORTED );
   EXPECT_EQ( second->terminations, 1 );
   for ( std::size_t i = 2; i < composite.getBehaviors().size(); ++i ) {
      EXPECT_EQ( composite.getBehaviors()[ i ]->getState(), State::READY );
   }
}
