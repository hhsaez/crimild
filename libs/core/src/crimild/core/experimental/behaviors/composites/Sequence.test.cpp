#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>
#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::behaviors::Behavior;
   using crimild::experimental::behaviors::composites::Sequence;
   using State = Behavior::State;

   class CallbackBehavior : public Behavior {
   public:
      std::function< State() > callback;

   protected:
      State update( void ) override
      {
         return callback();
      }
   };

   class RecordingBehavior : public Behavior {
   public:
      RecordingBehavior( int id, State result, std::vector< int > &calls )
         : result( result ), m_id( id ), m_calls( calls )
      {
      }

      State result;
      int initializations = 0;
      int terminations = 0;
      State terminatedWith = State::READY;

   protected:
      void initialize( void ) override { ++initializations; }

      State update( void ) override
      {
         m_calls.push_back( m_id );
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

   class ForwardingDecorator : public crimild::experimental::behaviors::decorators::Decorator {
   public:
      ForwardingDecorator( void ) = default;

   protected:
      State update( void ) override { return getBehavior()->tick(); }
   };

}

TEST( experimental_Sequence, empty_sequence_succeeds )
{
   Sequence sequence;
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( sequence.getState(), State::SUCCESS );
}

TEST( experimental_Sequence, ticks_successful_children_in_order )
{
   Sequence sequence;
   std::vector< int > calls;
   for ( int id : { 1, 2, 3 } ) {
      sequence.attach( std::make_shared< RecordingBehavior >( id, State::SUCCESS, calls ) );
   }
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_Sequence, failure_stops_later_children )
{
   Sequence sequence;
   std::vector< int > calls;
   sequence.attach( std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls ) );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::FAILURE, calls ) );
   auto later = std::make_shared< RecordingBehavior >( 3, State::SUCCESS, calls );
   sequence.attach( later );

   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( sequence.getState(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_Sequence, running_child_stops_later_children )
{
   Sequence sequence;
   std::vector< int > calls;
   sequence.attach( std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls ) );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls ) );
   // A later failure must not override the running child's result.
   auto later = std::make_shared< RecordingBehavior >( 3, State::FAILURE, calls );
   sequence.attach( later );

   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( sequence.getState(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_Sequence, proceeds_when_running_child_succeeds )
{
   Sequence sequence;
   std::vector< int > calls;
   auto child = std::make_shared< RecordingBehavior >( 1, State::RUNNING, calls );
   sequence.attach( child );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::SUCCESS, calls ) );

   EXPECT_EQ( sequence.tick(), State::RUNNING );
   child->result = State::SUCCESS;
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 1, 2 } ) );
}

TEST( experimental_Sequence, resumes_running_child_without_reevaluating_successful_children )
{
   Sequence sequence;
   std::vector< int > calls;
   auto condition = std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls );
   auto action = std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls );
   sequence.attach( condition );
   sequence.attach( action );
   sequence.attach( std::make_shared< RecordingBehavior >( 3, State::SUCCESS, calls ) );

   ASSERT_EQ( sequence.tick(), State::RUNNING );
   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 2 } ) );
   EXPECT_EQ( action->initializations, 1 );
   EXPECT_EQ( action->terminations, 0 );

   action->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 2, 3 } ) );
   EXPECT_EQ( action->initializations, 1 );
   EXPECT_EQ( action->terminations, 1 );

   calls.clear();
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
}

TEST( experimental_Sequence, attachment_during_execution_aborts_and_stops_traversal )
{
   Sequence sequence;
   std::vector< int > calls;
   auto mutator = std::make_shared< CallbackBehavior >();
   bool attached = false;
   mutator->callback = [ & ] {
      calls.push_back( 1 );
      if ( !attached ) {
         attached = true;
         const auto capacity = sequence.getBehaviors().capacity();
         while ( sequence.getBehaviors().size() <= capacity ) {
            sequence.attach( std::make_shared< RecordingBehavior >( 3, State::SUCCESS, calls ) );
         }
      }
      return State::SUCCESS;
   };
   sequence.attach( mutator );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::SUCCESS, calls ) );
   EXPECT_EQ( sequence.tick(), State::ABORTED );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( mutator->getState(), State::ABORTED );

   calls.clear();
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   std::vector< int > expected( sequence.getBehaviors().size(), 3 );
   expected[ 0 ] = 1;
   expected[ 1 ] = 2;
   EXPECT_EQ( calls, expected );
}

TEST( experimental_Sequence, restarts_from_first_child_after_abort )
{
   Sequence sequence;
   std::vector< int > calls;
   auto first = std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls );
   auto running = std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls );
   sequence.attach( first );
   sequence.attach( running );
   ASSERT_EQ( sequence.tick(), State::RUNNING );

   sequence.abort();
   sequence.abort();
   EXPECT_EQ( running->getState(), State::ABORTED );
   EXPECT_EQ( running->terminations, 1 );
   EXPECT_EQ( running->terminatedWith, State::ABORTED );

   calls.clear();
   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( running->initializations, 2 );
}

TEST( experimental_Sequence, running_child_failure_stops_and_next_activation_restarts )
{
   Sequence sequence;
   std::vector< int > calls;
   auto running = std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls );
   auto later = std::make_shared< RecordingBehavior >( 3, State::SUCCESS, calls );
   sequence.attach( std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls ) );
   sequence.attach( running );
   sequence.attach( later );
   ASSERT_EQ( sequence.tick(), State::RUNNING );

   running->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
   EXPECT_EQ( running->terminations, 1 );

   running->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_Sequence, attachment_while_running_aborts_and_restarts_with_new_children )
{
   Sequence sequence;
   std::vector< int > calls;
   auto running = std::make_shared< RecordingBehavior >( 1, State::RUNNING, calls );
   auto appended = std::make_shared< RecordingBehavior >( 2, State::SUCCESS, calls );
   sequence.attach( running );
   ASSERT_EQ( sequence.tick(), State::RUNNING );
   sequence.attach( appended );
   EXPECT_EQ( sequence.getState(), State::ABORTED );
   EXPECT_EQ( running->getState(), State::ABORTED );
   EXPECT_EQ( running->terminations, 1 );
   EXPECT_EQ( appended->getState(), State::READY );

   running->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
}

TEST( experimental_Sequence, abort_propagates_through_nested_sequences_and_decorators )
{
   Sequence sequence;
   std::vector< int > calls;
   auto inner = std::make_shared< Sequence >();
   auto decorator = std::make_shared< ForwardingDecorator >();
   auto action = std::make_shared< RecordingBehavior >( 1, State::RUNNING, calls );
   auto innerWaiting = std::make_shared< Behavior >();
   auto outerWaiting = std::make_shared< Behavior >();
   decorator->attach( action );
   inner->attach( decorator );
   inner->attach( innerWaiting );
   sequence.attach( inner );
   sequence.attach( outerWaiting );
   ASSERT_EQ( sequence.tick(), State::RUNNING );

   sequence.abort();

   EXPECT_EQ( sequence.getState(), State::ABORTED );
   EXPECT_EQ( inner->getState(), State::ABORTED );
   EXPECT_EQ( decorator->getState(), State::ABORTED );
   EXPECT_EQ( action->getState(), State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
   EXPECT_EQ( innerWaiting->getState(), State::READY );
   EXPECT_EQ( outerWaiting->getState(), State::READY );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
}
