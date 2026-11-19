#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>
#include <crimild/core/experimental/behaviors/composites/ActiveSequence.hpp>
#include <crimild/core/experimental/behaviors/composites/Composite.hpp>
#include <crimild/core/experimental/behaviors/composites/Parallel.hpp>
#include <crimild/core/experimental/behaviors/composites/Selector.hpp>
#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <type_traits>

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
      std::function< void() > onUpdate;
      int updates = 0;

   protected:
      State update( void ) override
      {
         ++updates;
         if ( onUpdate ) {
            onUpdate();
         }
         return result;
      }

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

TEST( experimental_Composite, rejects_cycles_without_interrupting_execution )
{
   auto parent = std::make_shared< ControlledComposite >();
   auto nested = std::make_shared< ControlledComposite >();
   auto child = std::make_shared< Behavior >();
   parent->attach( nested );
   nested->attach( child );
   ASSERT_EQ( parent->tick(), State::RUNNING );
   parent->attach( parent );
   nested->attach( parent );
   EXPECT_EQ( parent->getState(), State::RUNNING );
   EXPECT_FALSE( parent->hasParent() );
   ASSERT_EQ( parent->getBehaviors().size(), 1 );
   EXPECT_EQ( parent->getBehaviors().front(), nested );
   ASSERT_EQ( nested->getBehaviors().size(), 1 );
   EXPECT_EQ( nested->getBehaviors().front(), child );
   EXPECT_EQ( child->getParent(), nested );
   EXPECT_EQ( nested->getParent(), parent );
   parent->abort();
}

TEST( experimental_Composite, duplicate_attachment_and_unrelated_detachment_are_no_ops )
{
   ControlledComposite parent;
   auto child = std::make_shared< TrackedBehavior >();
   parent.attach( child );
   child->tick();
   parent.tick();
   parent.attach( child );
   parent.attach( nullptr );
   parent.detach( std::make_shared< Behavior >() );
   EXPECT_EQ( parent.getBehaviors().size(), 1 );
   EXPECT_EQ( parent.getState(), State::RUNNING );
   EXPECT_EQ( child->getState(), State::RUNNING );
   parent.abort();
}

TEST( experimental_Composite, explicit_detachment_cancels_before_moving_child )
{
   ControlledComposite parent;
   auto destination = std::make_shared< TestComposite >();
   auto child = std::make_shared< TrackedBehavior >();
   parent.attach( child );
   child->tick();
   parent.tick();
   parent.detach( parent.getBehaviors().front() );
   EXPECT_EQ( parent.getState(), State::ABORTED );
   EXPECT_TRUE( parent.getBehaviors().empty() );
   EXPECT_FALSE( child->hasParent() );
   EXPECT_EQ( child->terminations, 1 );
   destination->attach( child );
   EXPECT_EQ( child->getParent(), destination );
}

TEST( experimental_Composite, destruction_clears_parent_of_retained_child )
{
   auto child = std::make_shared< Behavior >();
   {
      auto parent = std::make_shared< TestComposite >();
      parent->attach( child );
      EXPECT_EQ( child->getParent(), parent );
   }
   EXPECT_EQ( child->getParent(), nullptr );
   auto destination = std::make_shared< TestComposite >();
   destination->attach( child );
   EXPECT_EQ( child->getParent(), destination );
}

namespace {

   template< class T >
   class experimental_CompositeCancellation : public testing::Test { };
   using CompositeTypes = testing::Types<
      crimild::experimental::behaviors::composites::Sequence,
      crimild::experimental::behaviors::composites::Selector,
      crimild::experimental::behaviors::composites::ActiveSelector,
      crimild::experimental::behaviors::composites::ActiveSequence,
      crimild::experimental::behaviors::composites::Parallel >;
   TYPED_TEST_SUITE( experimental_CompositeCancellation, CompositeTypes );

}

TYPED_TEST( experimental_CompositeCancellation, edits_during_first_and_later_ticks_stop_traversal_and_restart )
{
   for ( const bool firstTick : { true, false } ) {
      for ( const bool detach : { true, false } ) {
         TypeParam parent;
         auto child = std::make_shared< TrackedBehavior >();
         auto later = std::make_shared< TrackedBehavior >();
         auto appended = std::make_shared< TrackedBehavior >();
         parent.attach( child );
         parent.attach( later );
         if ( !firstTick ) {
            ASSERT_EQ( parent.tick(), State::RUNNING );
         }
         const auto previousUpdates = later->updates;
         child->onUpdate = [ & ] {
            if ( detach ) {
               parent.detach( child );
            } else {
               parent.attach( appended );
            }
         };
         EXPECT_EQ( parent.tick(), State::ABORTED );
         EXPECT_EQ( child->terminations, 1 );
         EXPECT_EQ( child->terminatedWith, State::ABORTED );
         EXPECT_EQ( later->updates, previousUpdates );
         EXPECT_EQ( appended->updates, 0 );
         if ( detach ) {
            EXPECT_FALSE( child->hasParent() );
         }
         child->onUpdate = nullptr;
         EXPECT_EQ( parent.tick(), State::RUNNING );
         parent.abort();
      }
   }
}

TYPED_TEST( experimental_CompositeCancellation, child_abort_cancels_parent_instead_of_returning_a_result_from_update )
{
   TypeParam parent;
   auto child = std::make_shared< TrackedBehavior >();
   auto later = std::make_shared< TrackedBehavior >();
   parent.attach( child );
   parent.attach( later );
   child->onUpdate = [ & ] {
      child->abort();
   };
   EXPECT_EQ( parent.tick(), State::ABORTED );
   EXPECT_EQ( later->updates, 0 );
   EXPECT_EQ( child->terminations, 1 );
}

TEST( experimental_Composite, returned_parent_retains_owner_without_creating_a_cycle )
{
   auto child = std::make_shared< Behavior >();
   auto parent = std::make_shared< TestComposite >();
   std::weak_ptr< Behavior > weakParent = parent;
   parent->attach( child );
   auto retainedParent = child->getParent();
   parent.reset();
   EXPECT_FALSE( weakParent.expired() );
   EXPECT_EQ( child->getParent(), retainedParent );
   retainedParent.reset();
   EXPECT_TRUE( weakParent.expired() );
   EXPECT_EQ( child->getParent(), nullptr );
}

TEST( experimental_Composite, attachment_automatically_reparents_and_cancels_both_active_owners )
{
   ControlledComposite source;
   ControlledComposite destination;
   auto child = std::make_shared< TrackedBehavior >();
   auto sibling = std::make_shared< TrackedBehavior >();
   source.attach( child );
   destination.attach( sibling );
   child->tick();
   sibling->tick();
   source.tick();
   destination.tick();
   destination.attach( source.getBehaviors().front() );
   EXPECT_TRUE( source.getBehaviors().empty() );
   ASSERT_EQ( destination.getBehaviors().size(), 2 );
   EXPECT_EQ( destination.getBehaviors().back(), child );
   EXPECT_EQ( source.getState(), State::ABORTED );
   EXPECT_EQ( destination.getState(), State::ABORTED );
   EXPECT_EQ( child->terminations, 1 );
   EXPECT_EQ( sibling->terminations, 1 );
   source.detach( child );
   EXPECT_EQ( destination.getBehaviors().size(), 2 );
   destination.detach( child );
   EXPECT_FALSE( child->hasParent() );
}

TEST( experimental_Composite, detach_from_parent_retains_the_child_and_handles_an_unattached_behavior )
{
   TestComposite parent;
   auto child = std::make_shared< Behavior >();
   EXPECT_EQ( child->detachFromParent(), child );
   parent.attach( child );
   std::weak_ptr< Behavior > weakChild = child;
   child.reset();
   auto detached = parent.getBehaviors().front()->detachFromParent();
   EXPECT_TRUE( parent.getBehaviors().empty() );
   EXPECT_FALSE( detached->hasParent() );
   EXPECT_EQ( weakChild.lock(), detached );
   detached.reset();
   EXPECT_TRUE( weakChild.expired() );
}

TYPED_TEST( experimental_CompositeCancellation, reparenting_the_current_child_stops_the_old_traversal )
{
   TypeParam source;
   auto destination = std::make_shared< TestComposite >();
   auto child = std::make_shared< TrackedBehavior >();
   auto later = std::make_shared< TrackedBehavior >();
   source.attach( child );
   source.attach( later );
   child->onUpdate = [ & ] {
      destination->attach( child );
   };
   EXPECT_EQ( source.tick(), State::ABORTED );
   EXPECT_EQ( child->terminations, 1 );
   EXPECT_EQ( child->getParent(), destination );
   EXPECT_EQ( later->updates, 0 );
   ASSERT_EQ( source.getBehaviors().size(), 1 );
   EXPECT_EQ( source.getBehaviors().front(), later );
}
