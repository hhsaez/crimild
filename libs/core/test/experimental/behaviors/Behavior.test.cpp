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

TEST( ExperimentalBehavior, executes_again_while_running )
{
   TestBehavior behavior;
   EXPECT_EQ( behavior.execute(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.execute(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.executions, 2 );
}

TEST( ExperimentalBehavior, execution_state_is_independent_between_instances )
{
   TestBehavior first;
   TestBehavior second;
   first.execute();
   EXPECT_EQ( first.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( second.getState(), Behavior::State::READY );
   EXPECT_EQ( second.executions, 0 );
}
