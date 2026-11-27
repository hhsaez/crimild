#include <crimild/core/experimental/nodes/events/OnTick.hpp>
#include <crimild/core/experimental/nodes/visitors/Tick.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::Blackboard;
   using crimild::experimental::OnTick;
   using crimild::experimental::behaviors::Behavior;
   namespace keys = crimild::experimental::keys;

   class ReadClock : public Behavior {
   public:
      int starts = 0;
      std::vector< double > deltas;

   protected:
      void initialize( Blackboard &board ) override
      {
         Behavior::initialize( board );
         ++starts;
         EXPECT_TRUE( board.get( keys::Clock ) );
      }

      State update( Blackboard &board ) override
      {
         const auto clock = board.get( keys::Clock );
         EXPECT_TRUE( clock );
         if ( clock ) {
            deltas.push_back( clock->getDeltaTime() );
         }
         return State::RUNNING;
      }
   };

}

TEST( experimental_OnTick, publishes_clock_before_initialization_and_updates_each_tick )
{
   OnTick event;
   const auto action = std::make_shared< ReadClock >();
   event.setBehavior( action );
   crimild::Clock clock( 0.125 );
   clock.setAccumTime( 2.0 );
   EXPECT_EQ( event.trigger( clock ), Behavior::State::RUNNING );
   clock.setDeltaTime( 0.25 );
   EXPECT_EQ( event.trigger( clock ), Behavior::State::RUNNING );
   EXPECT_EQ( action->starts, 1 );
   EXPECT_EQ( action->deltas, ( std::vector< double > { 0.125, 0.25 } ) );
   const auto stored = event.getBlackboard()->get( keys::Clock );
   ASSERT_TRUE( stored );
   EXPECT_DOUBLE_EQ( stored->getAccumTime(), 2.0 );
   clock.setAccumTime( 9.0 );
   EXPECT_DOUBLE_EQ( event.getBlackboard()->get( keys::Clock )->getAccumTime(), 2.0 );
}

TEST( experimental_OnTick, retains_existing_signal_subscribers_after_behavior_execution )
{
   OnTick event;
   const auto action = std::make_shared< ReadClock >();
   event.setBehavior( action );
   int signals = 0;
   event.triggered.bind( [ & ]( const crimild::Clock &clock ) {
      ++signals;
      ASSERT_EQ( action->deltas.size(), 1u );
      EXPECT_DOUBLE_EQ( action->deltas.back(), clock.getDeltaTime() );
   } );
   event.trigger( crimild::Clock( 0.5 ) );
   EXPECT_EQ( signals, 1 );
}

TEST( experimental_OnTick, events_without_behavior_still_publish_clock_and_notify_subscribers )
{
   OnTick event;
   int signals = 0;
   event.triggered.bind( [ & ]( const crimild::Clock & ) { ++signals; } );
   EXPECT_EQ( event.trigger( crimild::Clock( 0.5 ) ), Behavior::State::SUCCESS );
   EXPECT_EQ( signals, 1 );
   EXPECT_DOUBLE_EQ( event.getBlackboard()->get( keys::Clock )->getDeltaTime(), 0.5 );
}

TEST( experimental_OnTick, disabled_events_publish_nothing_and_notify_nobody )
{
   OnTick event;
   int signals = 0;
   event.triggered.bind( [ & ]( const crimild::Clock & ) { ++signals; } );
   event.setEnabled( false );
   EXPECT_EQ( event.trigger( crimild::Clock( 0.5 ) ), Behavior::State::SUCCESS );
   EXPECT_FALSE( event.getBlackboard()->contains( keys::Clock ) );
   EXPECT_EQ( signals, 0 );
}

TEST( experimental_OnTick, visitor_dispatches_the_clock_overload_virtually )
{
   class SpecializedTick : public OnTick {
   public:
      int calls = 0;
      State trigger( const crimild::Clock &clock ) override
      {
         ++calls;
         return OnTick::trigger( clock );
      }
   };
   SpecializedTick event;
   const auto action = std::make_shared< ReadClock >();
   event.setBehavior( action );
   event.perform< crimild::experimental::Tick >( crimild::Clock( 0.25 ) );
   EXPECT_EQ( event.calls, 1 );
   EXPECT_EQ( action->deltas, ( std::vector< double > { 0.25 } ) );
}
