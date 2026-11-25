#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>
#include <crimild/core/experimental/behaviors/composites/ActiveSequence.hpp>
#include <crimild/core/experimental/behaviors/composites/Parallel.hpp>
#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::Blackboard;
   using crimild::experimental::behaviors::Behavior;
   using crimild::experimental::behaviors::composites::ActiveSelector;
   using crimild::experimental::behaviors::composites::ActiveSequence;
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
      std::function< void() > onTerminate;

   protected:
      void initialize( Blackboard &blackboard ) override
      {
         Behavior::initialize( blackboard );
         ++initializations;
      }

      State update( Blackboard &blackboard ) override
      {
         m_calls.push_back( m_id );
         if ( callback ) {
            callback();
         }
         return result;
      }

      void terminate( State state ) override
      {
         ++terminations;
         terminatedWith = state;
         EXPECT_EQ( state, getState() );
         if ( onTerminate ) {
            onTerminate();
         }
      }

   private:
      int m_id;
      std::vector< int > &m_calls;
   };

}

TEST( experimental_ActiveSequence, empty_sequence_succeeds )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   EXPECT_EQ( sequence.tick( *blackboard ), State::SUCCESS );
}

TEST( experimental_ActiveSequence, all_successes_complete_in_attachment_order )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   for ( int id : { 1, 2, 3 } ) {
      sequence.attach( std::make_shared< Child >( id, State::SUCCESS, calls ) );
   }
   EXPECT_EQ( sequence.tick( *blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_ActiveSequence, failure_or_running_stops_before_unstarted_later_children )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( auto result : { State::FAILURE, State::RUNNING } ) {
      ActiveSequence sequence;
      std::vector< int > calls;
      auto blocker = std::make_shared< Child >( 2, result, calls );
      auto later = std::make_shared< Child >( 3, State::SUCCESS, calls );
      sequence.attach( std::make_shared< Child >( 1, State::SUCCESS, calls ) );
      sequence.attach( blocker );
      sequence.attach( later );
      EXPECT_EQ( sequence.tick( *blackboard ), result );
      EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
      EXPECT_EQ( later->getState(), State::READY );
      EXPECT_EQ( later->initializations, 0 );
   }
}

TEST( experimental_ActiveSequence, rechecks_successful_conditions_without_reinitializing_the_running_child )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   sequence.attach( condition );
   sequence.attach( action );
   ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );

   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( condition->initializations, 2 );
   EXPECT_EQ( action->initializations, 1 );
   EXPECT_EQ( action->terminations, 0 );
}

TEST( experimental_ActiveSequence, nested_remembered_sequence_preserves_action_progress_until_condition_fails )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto actions = std::make_shared< Sequence >();
   auto aim = std::make_shared< Child >( 2, State::SUCCESS, calls );
   auto burst = std::make_shared< Child >( 3, State::RUNNING, calls );
   actions->attach( aim );
   actions->attach( burst );
   sequence.attach( condition );
   sequence.attach( actions );
   ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );

   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 3 } ) );
   EXPECT_EQ( aim->initializations, 1 );
   EXPECT_EQ( burst->initializations, 1 );

   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( actions->getState(), State::ABORTED );
   EXPECT_EQ( burst->terminatedWith, State::ABORTED );
   EXPECT_EQ( burst->terminations, 1 );
   EXPECT_EQ( aim->terminatedWith, State::SUCCESS );
}

TEST( experimental_ActiveSequence, earlier_running_child_aborts_later_work_and_waits_instead_of_failing )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto actions = std::make_shared< Sequence >();
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   actions->attach( action );
   sequence.attach( first );
   sequence.attach( actions );
   ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );

   first->result = State::RUNNING;
   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( actions->getState(), State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( action->terminations, 1 );

   first->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( action->initializations, 2 );
}

TEST( experimental_ActiveSequence, directly_attached_successful_actions_are_repeated_on_each_tick )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   auto completed = std::make_shared< Child >( 1, State::SUCCESS, calls );
   sequence.attach( completed );
   sequence.attach( std::make_shared< Child >( 2, State::RUNNING, calls ) );
   ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( completed->initializations, 2 );
}

TEST( experimental_ActiveSequence, each_condition_is_checked_before_work_and_failure_short_circuits )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto second = std::make_shared< Child >( 2, State::SUCCESS, calls );
   auto action = std::make_shared< Child >( 3, State::RUNNING, calls );
   sequence.attach( first );
   sequence.attach( second );
   sequence.attach( action );
   ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   second->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );

   first->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( action->terminations, 1 );
}

TEST( experimental_ActiveSequence, running_child_can_complete_without_waiting_an_extra_tick )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   auto action = std::make_shared< Child >( 1, State::RUNNING, calls );
   sequence.attach( action );
   sequence.attach( std::make_shared< Child >( 2, State::SUCCESS, calls ) );
   ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   action->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( action->initializations, 1 );
}

TEST( experimental_ActiveSequence, starts_fresh_after_success_or_failure )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( auto result : { State::SUCCESS, State::FAILURE } ) {
      ActiveSequence sequence;
      std::vector< int > calls;
      auto condition = std::make_shared< Child >( 1, State::SUCCESS, calls );
      auto action = std::make_shared< Child >( 2, result, calls );
      sequence.attach( condition );
      sequence.attach( action );
      ASSERT_EQ( sequence.tick( *blackboard ), result );
      action->result = State::RUNNING;
      calls.clear();
      EXPECT_EQ( sequence.tick( *blackboard ), State::RUNNING );
      EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
      EXPECT_EQ( condition->initializations, 2 );
      EXPECT_EQ( action->initializations, 2 );
   }
}

TEST( experimental_ActiveSequence, external_abort_cleans_running_work_once_and_next_tick_restarts )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   auto action = std::make_shared< Child >( 1, State::RUNNING, calls );
   sequence.attach( action );
   ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   sequence.abort();
   sequence.abort();
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( action->initializations, 2 );
}

TEST( experimental_ActiveSequence, action_abort_cancels_parent_on_first_and_later_ticks )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( bool firstTick : { true, false } ) {
      ActiveSequence sequence;
      std::vector< int > calls;
      auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
      auto later = std::make_shared< Child >( 3, State::SUCCESS, calls );
      sequence.attach( std::make_shared< Child >( 1, State::SUCCESS, calls ) );
      sequence.attach( action );
      sequence.attach( later );
      if ( !firstTick ) {
         ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );
      }
      action->callback = [ & ] {
         action->abort();
      };
      EXPECT_EQ( sequence.tick( *blackboard ), State::ABORTED );
      EXPECT_EQ( action->terminatedWith, State::ABORTED );
      EXPECT_EQ( action->terminations, 1 );
      EXPECT_EQ( later->getState(), State::READY );
   }
}

TEST( experimental_ActiveSequence, cleanup_edit_aborts_traversal_and_preserves_a_stable_snapshot )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSequence sequence;
   std::vector< int > calls;
   auto guard = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto appended = std::make_shared< Child >( 3, State::SUCCESS, calls );
   sequence.attach( guard );
   sequence.attach( action );
   ASSERT_EQ( sequence.tick( *blackboard ), State::RUNNING );
   action->onTerminate = [ & ] {
      sequence.attach( appended );
   };
   guard->result = State::RUNNING;
   calls.clear();
   EXPECT_EQ( sequence.tick( *blackboard ), State::ABORTED );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( guard->terminatedWith, State::ABORTED );
   EXPECT_EQ( appended->getState(), State::READY );
}

TEST( experimental_ActiveSequence, failed_guard_allows_selector_fallback_in_the_same_tick )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector root;
   auto sequence = std::make_shared< ActiveSequence >();
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto fallback = std::make_shared< Child >( 3, State::RUNNING, calls );
   sequence->attach( condition );
   sequence->attach( action );
   root.attach( sequence );
   root.attach( fallback );
   ASSERT_EQ( root.tick( *blackboard ), State::RUNNING );
   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( root.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 3 } ) );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( sequence->getState(), State::FAILURE );
}

TEST( experimental_ActiveSequence, failed_guard_cancels_a_parallel_group_before_later_work_updates )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel root;
   auto sequence = std::make_shared< ActiveSequence >();
   std::vector< int > calls;
   auto condition = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto radio = std::make_shared< Child >( 3, State::RUNNING, calls );
   sequence->attach( condition );
   sequence->attach( action );
   root.attach( sequence );
   root.attach( radio );
   ASSERT_EQ( root.tick( *blackboard ), State::RUNNING );
   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( root.tick( *blackboard ), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( radio->terminatedWith, State::ABORTED );
   EXPECT_EQ( radio->terminations, 1 );
}
