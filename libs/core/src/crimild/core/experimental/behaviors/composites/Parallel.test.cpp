#include <crimild/core/experimental/behaviors/composites/Parallel.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::Blackboard;
   using crimild::experimental::behaviors::Behavior;
   using crimild::experimental::behaviors::composites::Parallel;
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
      void initialize( std::shared_ptr< Blackboard > const &blackboard ) override
      {
         Behavior::initialize( blackboard );
         ++initializations;
      }

      State update( std::shared_ptr< Blackboard > const &blackboard ) override
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

TEST( experimental_Parallel, empty_parallel_succeeds )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   EXPECT_EQ( parallel.tick( blackboard ), State::SUCCESS );
}

TEST( experimental_Parallel, all_successes_complete_in_attachment_order )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   std::vector< int > calls;
   for ( int id : { 1, 2, 3 } ) {
      parallel.attach( std::make_shared< Child >( id, State::SUCCESS, calls ) );
   }
   EXPECT_EQ( parallel.tick( blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_Parallel, ticks_all_unfinished_children_and_skips_completed_ones )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto second = std::make_shared< Child >( 2, State::SUCCESS, calls );
   auto third = std::make_shared< Child >( 3, State::RUNNING, calls );
   parallel.attach( first );
   parallel.attach( second );
   parallel.attach( third );
   ASSERT_EQ( parallel.tick( blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );

   first->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( parallel.tick( blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 3 } ) );
   EXPECT_EQ( second->initializations, 1 );
   EXPECT_EQ( third->initializations, 1 );
   EXPECT_EQ( third->terminations, 0 );

   third->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( parallel.tick( blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 3 } ) );
   EXPECT_EQ( third->terminations, 1 );
}

TEST( experimental_Parallel, failure_stops_traversal_and_aborts_running_siblings )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto failing = std::make_shared< Child >( 2, State::RUNNING, calls );
   auto later = std::make_shared< Child >( 3, State::RUNNING, calls );
   parallel.attach( first );
   parallel.attach( failing );
   parallel.attach( later );
   ASSERT_EQ( parallel.tick( blackboard ), State::RUNNING );

   failing->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( parallel.tick( blackboard ), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( first->terminatedWith, State::ABORTED );
   EXPECT_EQ( later->terminatedWith, State::ABORTED );
   EXPECT_EQ( failing->terminatedWith, State::FAILURE );
   EXPECT_EQ( first->terminations, 1 );
   EXPECT_EQ( later->terminations, 1 );
   EXPECT_EQ( failing->terminations, 1 );
}

TEST( experimental_Parallel, early_failure_leaves_unstarted_children_ready )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   std::vector< int > calls;
   parallel.attach( std::make_shared< Child >( 1, State::FAILURE, calls ) );
   auto later = std::make_shared< Child >( 2, State::SUCCESS, calls );
   parallel.attach( later );
   EXPECT_EQ( parallel.tick( blackboard ), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_Parallel, abort_propagates_to_nested_running_children_once )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   std::vector< int > calls;
   auto nested = std::make_shared< Parallel >();
   auto action = std::make_shared< Child >( 1, State::RUNNING, calls );
   auto sibling = std::make_shared< Child >( 2, State::RUNNING, calls );
   nested->attach( action );
   parallel.attach( nested );
   parallel.attach( sibling );
   ASSERT_EQ( parallel.tick( blackboard ), State::RUNNING );
   parallel.abort();
   parallel.abort();
   EXPECT_EQ( nested->getState(), State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( sibling->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( sibling->terminations, 1 );
}

TEST( experimental_Parallel, next_activation_reruns_previously_successful_children )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   std::vector< int > calls;
   auto child = std::make_shared< Child >( 1, State::SUCCESS, calls );
   ASSERT_EQ( child->tick( blackboard ), State::SUCCESS );
   parallel.attach( child );
   calls.clear();
   EXPECT_EQ( parallel.tick( blackboard ), State::SUCCESS );
   EXPECT_EQ( parallel.tick( blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 1 } ) );
   EXPECT_EQ( child->initializations, 3 );
   EXPECT_EQ( child->terminations, 3 );
}

TEST( experimental_Parallel, restarts_after_failure_and_abort )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   std::vector< int > calls;
   auto first = std::make_shared< Child >( 1, State::SUCCESS, calls );
   auto second = std::make_shared< Child >( 2, State::FAILURE, calls );
   parallel.attach( first );
   parallel.attach( second );
   ASSERT_EQ( parallel.tick( blackboard ), State::FAILURE );

   second->result = State::RUNNING;
   calls.clear();
   EXPECT_EQ( parallel.tick( blackboard ), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   parallel.abort();

   second->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( parallel.tick( blackboard ), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( first->initializations, 3 );
   EXPECT_EQ( second->initializations, 3 );
}

TEST( experimental_Parallel, attachment_during_update_aborts_and_stops_traversal )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Parallel parallel;
   std::vector< int > calls;
   auto mutator = std::make_shared< Child >( 1, State::SUCCESS, calls );
   bool attached = false;
   mutator->callback = [ & ] {
      if ( !attached ) {
         attached = true;
         const auto capacity = parallel.getBehaviors().capacity();
         while ( parallel.getBehaviors().size() <= capacity ) {
            parallel.attach( std::make_shared< Child >( 3, State::SUCCESS, calls ) );
         }
      }
   };
   parallel.attach( mutator );
   parallel.attach( std::make_shared< Child >( 2, State::SUCCESS, calls ) );
   EXPECT_EQ( parallel.tick( blackboard ), State::ABORTED );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( mutator->terminations, 1 );

   calls.clear();
   EXPECT_EQ( parallel.tick( blackboard ), State::SUCCESS );
   std::vector< int > expected( parallel.getBehaviors().size(), 3 );
   expected[ 0 ] = 1;
   expected[ 1 ] = 2;
   EXPECT_EQ( calls, expected );
}
