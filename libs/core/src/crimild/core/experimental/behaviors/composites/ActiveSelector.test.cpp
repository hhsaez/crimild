#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::Blackboard;
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
      }

   private:
      int m_id;
      std::vector< int > &m_calls;
   };

}

TEST( experimental_ActiveSelector, empty_selector_fails )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   EXPECT_EQ( selector.tick( *blackboard ), State::FAILURE );
}

TEST( experimental_ActiveSelector, all_failures_are_visited_in_order )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   std::vector< int > calls;
   for ( int id : { 1, 2, 3 } ) {
      selector.attach( std::make_shared< Child >( id, State::FAILURE, calls ) );
   }
   EXPECT_EQ( selector.tick( *blackboard ), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_ActiveSelector, success_avoids_execution_of_later_children )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   std::vector< int > calls;
   selector.attach( std::make_shared< Child >( 1, State::FAILURE, calls ) );
   selector.attach( std::make_shared< Child >( 2, State::SUCCESS, calls ) );
   auto later = std::make_shared< Child >( 3, State::SUCCESS, calls );
   selector.attach( later );
   EXPECT_EQ( selector.tick( *blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_ActiveSelector, reevaluates_priority_without_reinitializing_the_running_child )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto running = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto later = std::make_shared< Child >( 3, State::SUCCESS, calls );
   selector.attach( first );
   selector.attach( running );
   selector.attach( later );
   ASSERT_EQ( selector.tick( *blackboard ), State::RUNNING );
   calls.clear();
   EXPECT_EQ( selector.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( first->initializations, 2 );
   EXPECT_EQ( running->initializations, 1 );
   EXPECT_EQ( running->terminations, 0 );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_ActiveSelector, higher_priority_running_child_aborts_displaced_child_once )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto later = std::make_shared< Child >( 2, State::RUNNING, calls );
   selector.attach( first );
   selector.attach( later );
   ASSERT_EQ( selector.tick( *blackboard ), State::RUNNING );
   first->result = State::RUNNING;
   calls.clear();
   EXPECT_EQ( selector.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( later->getState(), State::ABORTED );
   EXPECT_EQ( later->terminatedWith, State::ABORTED );
   EXPECT_EQ( later->terminations, 1 );
   EXPECT_EQ( selector.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( later->terminations, 1 );

   first->result = State::FAILURE;
   EXPECT_EQ( selector.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( later->initializations, 2 );
}

TEST( experimental_ActiveSelector, higher_priority_success_cancels_running_descendants )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::FAILURE, calls );
   auto nested = std::make_shared< ActiveSelector >();
   auto action = std::make_shared< Child >( 2, State::RUNNING, calls );
   nested->attach( action );
   selector.attach( first );
   selector.attach( nested );
   ASSERT_EQ( selector.tick( *blackboard ), State::RUNNING );

   first->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( selector.tick( *blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( nested->getState(), State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
}

TEST( experimental_ActiveSelector, running_child_failure_advances_to_next_child_in_same_tick )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::RUNNING, calls );
   selector.attach( first );
   selector.attach( std::make_shared< Child >( 2, State::SUCCESS, calls ) );
   ASSERT_EQ( selector.tick( *blackboard ), State::RUNNING );
   first->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( selector.tick( *blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
}

TEST( experimental_ActiveSelector, abort_cleans_up_once_and_next_tick_restarts )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   std::vector< int > calls;
   auto running = std::make_shared< Child >( 1, State::RUNNING, calls );
   selector.attach( running );
   ASSERT_EQ( selector.tick( *blackboard ), State::RUNNING );
   selector.abort();
   selector.abort();
   EXPECT_EQ( running->terminatedWith, State::ABORTED );
   EXPECT_EQ( running->terminations, 1 );
   EXPECT_EQ( selector.tick( *blackboard ), State::RUNNING );
   EXPECT_EQ( running->initializations, 2 );
}

TEST( experimental_ActiveSelector, attachment_during_update_aborts_and_stops_selection )
{
   const auto blackboard = std::make_shared< Blackboard >();
   ActiveSelector selector;
   std::vector< int > calls;
   auto mutator = std::make_shared< Child >( 1, State::FAILURE, calls );
   bool attached = false;
   mutator->callback = [ & ] {
      if ( !attached ) {
         attached = true;
         const auto capacity = selector.getBehaviors().capacity();
         while ( selector.getBehaviors().size() <= capacity ) {
            selector.attach( std::make_shared< Child >( 3, State::SUCCESS, calls ) );
         }
      }
   };
   selector.attach( mutator );
   selector.attach( std::make_shared< Child >( 2, State::FAILURE, calls ) );
   EXPECT_EQ( selector.tick( *blackboard ), State::ABORTED );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( mutator->terminations, 1 );
   calls.clear();
   EXPECT_EQ( selector.tick( *blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}
