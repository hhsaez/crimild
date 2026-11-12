#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <gtest/gtest.h>

namespace {

   using crimild::experimental::behaviors::Behavior;

   class TestBehavior : public Behavior {
   public:
      State result = State::RUNNING;
      int updates = 0;

      State update( void ) override
      {
         ++updates;
         return result;
      }
   };

}

TEST( ExperimentalBehavior, supports_naming )
{
   Behavior behavior;
   EXPECT_TRUE( behavior.getName().empty() );

   behavior.setName( "Patrol" );
   EXPECT_EQ( behavior.getName(), "Patrol" );

   behavior.setName( "Chase" );
   EXPECT_EQ( behavior.getName(), "Chase" );
}

TEST( ExperimentalBehavior, starts_ready_without_ticking )
{
   TestBehavior behavior;
   EXPECT_EQ( behavior.getState(), Behavior::State::READY );
   EXPECT_EQ( behavior.updates, 0 );
}

TEST( ExperimentalBehavior, default_tick_succeeds )
{
   Behavior behavior;
   EXPECT_EQ( behavior.tick(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
}

TEST( ExperimentalBehavior, stores_each_tick_result )
{
   TestBehavior behavior;
   for ( const auto state : { Behavior::State::RUNNING, Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      behavior.result = state;
      EXPECT_EQ( behavior.tick(), state );
      EXPECT_EQ( behavior.getState(), state );
   }
   EXPECT_EQ( behavior.updates, 3 );
}

TEST( ExperimentalBehavior, keeps_ticking_running_behavior_until_success )
{
   TestBehavior behavior;
   EXPECT_EQ( behavior.getState(), Behavior::State::READY );

   // update() requests another tick until the action reports completion.
   EXPECT_EQ( behavior.tick(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.tick(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.updates, 2 );

   behavior.result = Behavior::State::SUCCESS;
   EXPECT_EQ( behavior.tick(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.updates, 3 );
}
