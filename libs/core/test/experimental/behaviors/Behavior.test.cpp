#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

   using crimild::experimental::behaviors::Behavior;

   class TestBehavior : public Behavior {
   public:
      State result = State::RUNNING;
      int updates = 0;

   protected:
      State update( void ) override
      {
         ++updates;
         return result;
      }
   };

   class LifecycleBehavior : public TestBehavior {
   public:
      std::vector< std::string > calls;
      std::vector< State > terminationStates;

   protected:
      void initialize( void ) override
      {
         calls.push_back( "initialize" );
      }

      State update( void ) override
      {
         calls.push_back( "update" );
         return TestBehavior::update();
      }

      void terminate( void ) override
      {
         calls.push_back( "terminate" );
         terminationStates.push_back( getState() );
      }
   };

}

TEST( experimental_Behavior, supports_naming )
{
   Behavior behavior;
   EXPECT_TRUE( behavior.getName().empty() );

   behavior.setName( "Patrol" );
   EXPECT_EQ( behavior.getName(), "Patrol" );

   behavior.setName( "Chase" );
   EXPECT_EQ( behavior.getName(), "Chase" );
}

TEST( experimental_Behavior, starts_ready_without_ticking )
{
   TestBehavior behavior;
   EXPECT_EQ( behavior.getState(), Behavior::State::READY );
   EXPECT_EQ( behavior.updates, 0 );
}

TEST( experimental_Behavior, default_tick_succeeds )
{
   Behavior behavior;
   EXPECT_EQ( behavior.tick(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
}

TEST( experimental_Behavior, stores_each_tick_result )
{
   TestBehavior behavior;
   for ( const auto state : { Behavior::State::RUNNING, Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      behavior.result = state;
      EXPECT_EQ( behavior.tick(), state );
      EXPECT_EQ( behavior.getState(), state );
   }
   EXPECT_EQ( behavior.updates, 3 );
}

TEST( experimental_Behavior, keeps_ticking_running_behavior_until_success )
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

TEST( experimental_Behavior, initializes_before_the_first_update )
{
   LifecycleBehavior behavior;
   EXPECT_TRUE( behavior.calls.empty() );

   EXPECT_EQ( behavior.tick(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update" } ) );
   EXPECT_TRUE( behavior.terminationStates.empty() );
}

TEST( experimental_Behavior, running_ticks_do_not_repeat_initialization_or_terminate )
{
   LifecycleBehavior behavior;
   for ( int tick = 0; tick < 3; ++tick ) {
      EXPECT_EQ( behavior.tick(), Behavior::State::RUNNING );
   }

   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "update", "update" } ) );
   EXPECT_TRUE( behavior.terminationStates.empty() );
}

TEST( experimental_Behavior, immediate_completion_terminates_with_the_stored_final_state )
{
   for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      LifecycleBehavior behavior;
      behavior.result = result;

      EXPECT_EQ( behavior.tick(), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );
   }
}

TEST( experimental_Behavior, running_activation_terminates_once_when_it_completes )
{
   for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      LifecycleBehavior behavior;
      ASSERT_EQ( behavior.tick(), Behavior::State::RUNNING );

      behavior.result = result;
      EXPECT_EQ( behavior.tick(), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );
   }
}

TEST( experimental_Behavior, ticking_after_completion_starts_a_new_activation )
{
   for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      LifecycleBehavior behavior;
      behavior.result = result;
      ASSERT_EQ( behavior.tick(), result );

      behavior.result = Behavior::State::RUNNING;
      EXPECT_EQ( behavior.tick(), Behavior::State::RUNNING );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );

      behavior.result = result;
      EXPECT_EQ( behavior.tick(), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result, result } ) );
   }
}

TEST( experimental_Behavior, abort_terminates_a_running_activation_once_without_updating )
{
   LifecycleBehavior behavior;
   ASSERT_EQ( behavior.tick(), Behavior::State::RUNNING );

   behavior.abort();
   behavior.abort();

   EXPECT_EQ( behavior.getState(), Behavior::State::ABORTED );
   EXPECT_EQ( behavior.updates, 1 );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate" } ) );
   EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}

TEST( experimental_Behavior, abort_before_activation_does_nothing )
{
   LifecycleBehavior behavior;
   behavior.abort();

   EXPECT_EQ( behavior.getState(), Behavior::State::READY );
   EXPECT_TRUE( behavior.calls.empty() );
}

TEST( experimental_Behavior, abort_after_completion_preserves_the_result_without_terminating_again )
{
   for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      LifecycleBehavior behavior;
      behavior.result = result;
      ASSERT_EQ( behavior.tick(), result );

      behavior.abort();

      EXPECT_EQ( behavior.getState(), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );
   }
}

TEST( experimental_Behavior, ticking_after_abort_starts_a_new_activation )
{
   LifecycleBehavior behavior;
   ASSERT_EQ( behavior.tick(), Behavior::State::RUNNING );
   behavior.abort();

   EXPECT_EQ( behavior.tick(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update" } ) );
   EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}
