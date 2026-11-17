#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::behaviors::Behavior;
   using crimild::experimental::behaviors::composites::ActiveSelector;
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

TEST( experimental_ActiveSelector, empty_selector_fails )
{
   ActiveSelector selector;
   EXPECT_EQ( selector.tick(), State::FAILURE );
}

TEST( experimental_ActiveSelector, all_failures_are_visited_in_order )
{
   ActiveSelector selector;
   std::vector< int > calls;
   for ( int id : { 1, 2, 3 } ) {
      selector.attach( std::make_shared< Child >( id, State::FAILURE, calls ) );
   }
   EXPECT_EQ( selector.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_ActiveSelector, success_stops_later_children )
{
   ActiveSelector selector;
   std::vector< int > calls;
   selector.attach( std::make_shared< Child >( 1, State::FAILURE, calls ) );
   selector.attach( std::make_shared< Child >( 2, State::SUCCESS, calls ) );
   auto later = std::make_shared< Child >( 3, State::SUCCESS, calls );
   selector.attach( later );
   EXPECT_EQ( selector.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_ActiveSelector, reevaluates_priority_without_reinitializing_the_running_child )
{
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto running = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto later = std::make_shared< Child >( 3, State::SUCCESS, calls );
   selector.attach( first );
   selector.attach( running );
   selector.attach( later );
   ASSERT_EQ( selector.tick(), State::RUNNING );
   calls.clear();
   EXPECT_EQ( selector.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( first->initializations, 2 );
   EXPECT_EQ( running->initializations, 1 );
   EXPECT_EQ( running->terminations, 0 );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_ActiveSelector, higher_priority_running_child_aborts_displaced_child_once )
{
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto later = std::make_shared< Child >( 2, State::RUNNING, calls );
   selector.attach( first );
   selector.attach( later );
   ASSERT_EQ( selector.tick(), State::RUNNING );
   first->result = State::RUNNING;
   calls.clear();
   EXPECT_EQ( selector.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( later->getState(), State::ABORTED );
   EXPECT_EQ( later->terminatedWith, State::ABORTED );
   EXPECT_EQ( later->terminations, 1 );
   EXPECT_EQ( selector.tick(), State::RUNNING );
   EXPECT_EQ( later->terminations, 1 );

   first->result = State::FAILURE;
   EXPECT_EQ( selector.tick(), State::RUNNING );
   EXPECT_EQ( later->initializations, 2 );
}

TEST( experimental_ActiveSelector, higher_priority_success_cancels_running_descendants )
{
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto nested = std::make_shared< ActiveSelector >();
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   nested->attach( action );
   selector.attach( first );
   selector.attach( nested );
   ASSERT_EQ( selector.tick(), State::RUNNING );

   first->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( selector.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( nested->getState(), State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
}

TEST( experimental_ActiveSelector, running_child_failure_advances_to_next_child_in_same_tick )
{
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::RUNNING, calls );
   selector.attach( first );
   selector.attach( std::make_shared< Child >( 2, State::SUCCESS, calls ) );
   ASSERT_EQ( selector.tick(), State::RUNNING );
   first->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( selector.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
}

TEST( experimental_ActiveSelector, abort_cleans_up_once_and_next_tick_restarts )
{
   ActiveSelector selector;
   std::vector< int > calls;
   auto running = std::make_shared< Child >( 1, State::RUNNING, calls );
   selector.attach( running );
   ASSERT_EQ( selector.tick(), State::RUNNING );
   selector.abort();
   selector.abort();
   EXPECT_EQ( running->terminatedWith, State::ABORTED );
   EXPECT_EQ( running->terminations, 1 );
   EXPECT_EQ( selector.tick(), State::RUNNING );
   EXPECT_EQ( running->initializations, 2 );
}

TEST( experimental_ActiveSelector, aborted_child_stops_selection_and_cancels_previous_work )
{
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto later = std::make_shared< Child >( 2, State::RUNNING, calls );
   selector.attach( first );
   selector.attach( later );
   ASSERT_EQ( selector.tick(), State::RUNNING );
   first->result = State::ABORTED;
   calls.clear();
   EXPECT_EQ( selector.tick(), State::ABORTED );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( later->terminatedWith, State::ABORTED );
}

TEST( experimental_ActiveSelector, attachments_during_update_are_deferred_until_next_tick )
{
   ActiveSelector selector;
   std::vector< int > calls;
   auto mutator = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto appended = std::make_shared< Child >( 3, State::SUCCESS, calls );
   bool attached = false;
   mutator->callback = [ & ] {
      if ( !attached ) {
         const auto capacity = selector.getBehaviors().capacity();
         while ( selector.getBehaviors().size() <= capacity ) {
            selector.attach( appended );
         }
         attached = true;
      }
   };
   auto running = std::make_shared< Child >( 2, State::RUNNING, calls );
   selector.attach( mutator );
   selector.attach( running );
   ASSERT_EQ( selector.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( appended->getState(), State::READY );

   running->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( selector.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_ActiveSelector, repeated_reference_to_selected_child_does_not_abort_it )
{
   ActiveSelector selector;
   std::vector< int > calls;
   auto running = std::make_shared< Child >( 1, State::RUNNING, calls );
   selector.attach( running );
   selector.attach( running );
   EXPECT_EQ( selector.tick(), State::RUNNING );
   EXPECT_EQ( running->terminations, 0 );
   selector.abort();
   EXPECT_EQ( running->terminations, 1 );
}
