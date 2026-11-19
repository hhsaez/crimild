#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>
#include <crimild/core/experimental/behaviors/composites/Monitor.hpp>
#include <crimild/core/experimental/behaviors/composites/Parallel.hpp>
#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::behaviors::Behavior;
   using crimild::experimental::behaviors::composites::ActiveSelector;
   using crimild::experimental::behaviors::composites::Monitor;
   using crimild::experimental::behaviors::composites::Parallel;
   using crimild::experimental::behaviors::composites::Sequence;
   using State = Behavior::State;

   class Child : public Behavior {
   public:
      Child( int id, State result, std::vector< int > &calls )
         : result( result ), m_id( id ), m_calls( calls )
      {
      }

      State result;
      int initializations = 0;
      int terminations = 0;
      State terminatedWith = State::READY;
      std::function< void() > callback;

   protected:
      void initialize( void ) override { ++initializations; }

      State update( void ) override
      {
         m_calls.push_back( m_id );
         if ( callback ) {
            callback();
         }
         return result;
      }

      void terminate( void ) override
      {
         ++terminations;
         terminatedWith = getState();
      }

   private:
      int m_id;
      std::vector< int > &m_calls;
   };

}

TEST( experimental_Monitor, requires_exactly_two_branches_without_ticking_partial_or_extra_work )
{
   for ( int count : { 0, 1, 3 } ) {
      SCOPED_TRACE( count );
      Monitor monitor;
      std::vector< int > calls;
      for ( int i = 0; i < count; ++i ) {
         monitor.attach( std::make_shared< Child >( i, State::RUNNING, calls ) );
      }
      EXPECT_EQ( monitor.tick(), State::FAILURE );
      EXPECT_TRUE( calls.empty() );
   }
}

TEST( experimental_Monitor, rechecks_running_condition_while_sequence_remembers_its_action )
{
   Monitor monitor;
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto sequence = std::make_shared< Sequence >();
   auto first = std::make_shared< Child >( 2, State::SUCCESS, calls );
   auto action = std::make_shared< Child >( 3, State::RUNNING, calls );
   sequence->attach( first );
   sequence->attach( action );
   monitor.attach( condition );
   monitor.attach( sequence );

   ASSERT_EQ( monitor.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
   calls.clear();
   EXPECT_EQ( monitor.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 3 } ) );
   EXPECT_EQ( condition->initializations, 1 );
   EXPECT_EQ( first->initializations, 1 );
   EXPECT_EQ( action->initializations, 1 );
   EXPECT_EQ( condition->terminations, 0 );
   EXPECT_EQ( action->terminations, 0 );
}

TEST( experimental_Monitor, rechecks_successful_instant_conditions_instead_of_caching_them )
{
   Monitor monitor;
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   monitor.attach( condition );
   monitor.attach( action );
   ASSERT_EQ( monitor.tick(), State::RUNNING );

   calls.clear();
   EXPECT_EQ( monitor.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( condition->initializations, 2 );
   EXPECT_EQ( action->initializations, 1 );

   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( monitor.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
}

TEST( experimental_Monitor, initial_condition_failure_leaves_action_unstarted )
{
   Monitor monitor;
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   monitor.attach( condition );
   monitor.attach( action );
   EXPECT_EQ( monitor.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( action->getState(), State::READY );
   EXPECT_EQ( action->initializations, 0 );
   EXPECT_EQ( condition->terminations, 1 );
}

TEST( experimental_Monitor, condition_failure_aborts_running_sequence_before_its_next_update )
{
   Monitor monitor;
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto sequence = std::make_shared< Sequence >();
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   sequence->attach( action );
   monitor.attach( condition );
   monitor.attach( sequence );
   ASSERT_EQ( monitor.tick(), State::RUNNING );

   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( monitor.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( sequence->getState(), State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( condition->terminatedWith, State::FAILURE );
   EXPECT_EQ( condition->terminations, 1 );
}

TEST( experimental_Monitor, action_completion_aborts_active_condition_and_preserves_action_result )
{
   for ( auto result : { State::SUCCESS, State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      Monitor monitor;
      std::vector< int > calls;
      auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
      auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
      monitor.attach( condition );
      monitor.attach( action );
      ASSERT_EQ( monitor.tick(), State::RUNNING );

      action->result = result;
      calls.clear();
      EXPECT_EQ( monitor.tick(), result );
      EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
      EXPECT_EQ( condition->terminatedWith, State::ABORTED );
      EXPECT_EQ( action->terminatedWith, result );
      EXPECT_EQ( condition->terminations, 1 );
      EXPECT_EQ( action->terminations, 1 );
   }
}

TEST( experimental_Monitor, parallel_monitoring_conditions_are_all_rechecked_and_any_failure_stops_work )
{
   Monitor monitor;
   std::vector< int > calls;
   auto conditions = std::make_shared< Parallel >();
   auto first = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto second = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto action = std::make_shared< Child >( 3, State::RUNNING, calls );
   conditions->attach( first );
   conditions->attach( second );
   monitor.attach( conditions );
   monitor.attach( action );
   ASSERT_EQ( monitor.tick(), State::RUNNING );
   calls.clear();
   EXPECT_EQ( monitor.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );

   second->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( monitor.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( first->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
}

TEST( experimental_Monitor, external_abort_cleans_both_branches_once_and_next_tick_restarts )
{
   Monitor monitor;
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   monitor.attach( condition );
   monitor.attach( action );
   ASSERT_EQ( monitor.tick(), State::RUNNING );
   monitor.abort();
   monitor.abort();
   EXPECT_EQ( condition->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( condition->terminations, 1 );
   EXPECT_EQ( action->terminations, 1 );

   calls.clear();
   EXPECT_EQ( monitor.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( condition->initializations, 2 );
   EXPECT_EQ( action->initializations, 2 );
}

TEST( experimental_Monitor, restarts_both_branches_after_success_or_failure )
{
   for ( auto result : { State::SUCCESS, State::FAILURE } ) {
      Monitor monitor;
      std::vector< int > calls;
      auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
      auto action = std::make_shared< Child >( 2, result, calls );
      monitor.attach( condition );
      monitor.attach( action );
      ASSERT_EQ( monitor.tick(), result );

      action->result = State::RUNNING;
      calls.clear();
      EXPECT_EQ( monitor.tick(), State::RUNNING );
      EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
      EXPECT_EQ( condition->initializations, 2 );
      EXPECT_EQ( action->initializations, 2 );
   }
}

TEST( experimental_Monitor, child_abort_cancels_both_branches_on_first_and_later_ticks )
{
   for ( int cancellingBranch : { 0, 1 } ) {
      for ( bool firstTick : { true, false } ) {
         SCOPED_TRACE( cancellingBranch );
         SCOPED_TRACE( firstTick );
         Monitor monitor;
         std::vector< int > calls;
         auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
         auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
         monitor.attach( condition );
         monitor.attach( action );
         if ( !firstTick ) {
            ASSERT_EQ( monitor.tick(), State::RUNNING );
         }
         const auto cancellingChild = cancellingBranch == 0 ? condition : action;
         cancellingChild->callback = [ & ] {
            cancellingChild->abort();
         };
         calls.clear();
         EXPECT_EQ( monitor.tick(), State::ABORTED );
         EXPECT_EQ( condition->terminatedWith, State::ABORTED );
         EXPECT_EQ( condition->terminations, 1 );
         if ( cancellingBranch == 0 && firstTick ) {
            EXPECT_EQ( action->getState(), State::READY );
            EXPECT_EQ( action->terminations, 0 );
         } else {
            EXPECT_EQ( action->terminatedWith, State::ABORTED );
            EXPECT_EQ( action->terminations, 1 );
         }
         EXPECT_EQ( calls, cancellingBranch == 0 ? ( std::vector< int > { 1 } ) : ( std::vector< int > { 1, 2 } ) );
      }
   }
}

TEST( experimental_Monitor, attachment_during_condition_tick_aborts_without_updating_action )
{
   Monitor monitor;
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto extra = std::make_shared< Child >( 3, State::RUNNING, calls );
   monitor.attach( condition );
   monitor.attach( action );
   ASSERT_EQ( monitor.tick(), State::RUNNING );
   condition->callback = [ & ] {
      monitor.attach( extra );
   };
   calls.clear();
   EXPECT_EQ( monitor.tick(), State::ABORTED );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( condition->terminations, 1 );
   EXPECT_EQ( action->terminations, 1 );

   monitor.detach( extra );
   condition->callback = nullptr;
   calls.clear();
   EXPECT_EQ( monitor.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
}

TEST( experimental_Monitor, reparenting_during_action_tick_preserves_abort_and_parent_links )
{
   auto monitor = std::make_shared< Monitor >();
   auto destination = std::make_shared< Sequence >();
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   monitor->attach( condition );
   monitor->attach( action );
   action->callback = [ & ] {
      destination->attach( action );
   };

   EXPECT_EQ( monitor->tick(), State::ABORTED );
   EXPECT_EQ( condition->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( action->getParent(), destination );
   EXPECT_EQ( monitor->getBehaviors().size(), 1 );
   calls.clear();
   EXPECT_EQ( monitor->tick(), State::FAILURE );
   EXPECT_TRUE( calls.empty() );
}

TEST( experimental_Monitor, failed_assumption_allows_selector_fallback_in_the_same_tick )
{
   ActiveSelector root;
   auto monitor = std::make_shared< Monitor >();
   auto sequence = std::make_shared< Sequence >();
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto fallback = std::make_shared< Child >( 3, State::RUNNING, calls );
   sequence->attach( action );
   monitor->attach( condition );
   monitor->attach( sequence );
   root.attach( monitor );
   root.attach( fallback );
   ASSERT_EQ( root.tick(), State::RUNNING );

   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( root.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 3 } ) );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( monitor->getState(), State::FAILURE );
   EXPECT_EQ( fallback->initializations, 1 );
}
