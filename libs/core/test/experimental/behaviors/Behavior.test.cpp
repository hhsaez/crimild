#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <gtest/gtest.h>

namespace {

   using crimild::experimental::behaviors::Behavior;

   class TestBehavior : public Behavior {
   public:
      State result = State::RUNNING;
      int executions = 0;

      State onExecute( void ) override
      {
         ++executions;
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

TEST( ExperimentalBehavior, starts_ready_without_executing )
{
   TestBehavior behavior;
   EXPECT_EQ( behavior.getState(), Behavior::State::READY );
   EXPECT_EQ( behavior.executions, 0 );
}

TEST( ExperimentalBehavior, default_execution_succeeds )
{
   Behavior behavior;
   EXPECT_EQ( behavior.execute(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
}

TEST( ExperimentalBehavior, stores_each_execution_result )
{
   TestBehavior behavior;
   for ( const auto state : { Behavior::State::RUNNING, Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      behavior.result = state;
      EXPECT_EQ( behavior.execute(), state );
      EXPECT_EQ( behavior.getState(), state );
   }
   EXPECT_EQ( behavior.executions, 3 );
}

TEST( ExperimentalBehavior, keeps_executing_running_behavior_until_success )
{
   TestBehavior behavior;
   EXPECT_EQ( behavior.getState(), Behavior::State::READY );

   // onExecute() requests another tick until the action reports completion.
   EXPECT_EQ( behavior.execute(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.execute(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.executions, 2 );

   behavior.result = Behavior::State::SUCCESS;
   EXPECT_EQ( behavior.execute(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.executions, 3 );
}

TEST( ExperimentalBehavior, execution_state_is_independent_between_instances )
{
   Behavior first;
   Behavior second;
   EXPECT_EQ( first.getState(), Behavior::State::READY );
   EXPECT_EQ( second.getState(), Behavior::State::READY );

   EXPECT_EQ( first.execute(), Behavior::State::SUCCESS );
   EXPECT_EQ( first.getState(), Behavior::State::SUCCESS );
   EXPECT_EQ( second.getState(), Behavior::State::READY );
}
