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
      int terminations = 0;
      State terminatedWith = State::READY;

   protected:
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

TEST( experimental_Sequence, reevaluates_earlier_children_on_each_execution )
{
   Sequence sequence;
   std::vector< int > calls;
   auto condition = std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls );
   auto action = std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls );
   sequence.attach( condition );
   sequence.attach( action );

   EXPECT_EQ( sequence.tick(), State::RUNNING );
   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( action->getState(), State::ABORTED );
   EXPECT_EQ( action->terminatedWith, State::ABORTED );
   EXPECT_EQ( action->terminations, 1 );
}

TEST( experimental_Sequence, defers_children_attached_during_execution_until_next_traversal )
{
   Sequence sequence;
   std::vector< int > calls;
   auto appended = std::make_shared< RecordingBehavior >( 3, State::SUCCESS, calls );
   auto mutator = std::make_shared< CallbackBehavior >();
   bool attached = false;
   mutator->callback = [ & ] {
      calls.push_back( 1 );
      if ( !attached ) {
         // Force reallocation of the original vector while it is being traversed.
         const auto capacity = sequence.getBehaviors().capacity();
         while ( sequence.getBehaviors().size() <= capacity ) {
            sequence.attach( appended );
         }
         attached = true;
      }
      return State::SUCCESS;
   };
   sequence.attach( mutator );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::SUCCESS, calls ) );

   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( appended->getState(), State::READY );

   calls.clear();
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   std::vector< int > expected( sequence.getBehaviors().size(), 3 );
   expected[ 0 ] = 1;
   expected[ 1 ] = 2;
   EXPECT_EQ( calls, expected );
}

TEST( experimental_Sequence, earlier_running_child_aborts_a_previously_running_later_child )
{
   Sequence sequence;
   std::vector< int > calls;
   auto earlier = std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls );
   auto later = std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls );
   sequence.attach( earlier );
   sequence.attach( later );
   ASSERT_EQ( sequence.tick(), State::RUNNING );

   earlier->result = State::RUNNING;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( sequence.getState(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( later->getState(), State::ABORTED );
   EXPECT_EQ( later->terminations, 1 );

   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( later->terminations, 1 );

   earlier->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::RUNNING );
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

TEST( experimental_Sequence, aborted_child_stops_the_sequence_without_starting_later_children )
{
   Sequence sequence;
   std::vector< int > calls;
   auto child = std::make_shared< RecordingBehavior >( 1, State::ABORTED, calls );
   auto later = std::make_shared< RecordingBehavior >( 2, State::SUCCESS, calls );
   sequence.attach( child );
   sequence.attach( later );

   EXPECT_EQ( sequence.tick(), State::ABORTED );
   EXPECT_EQ( sequence.getState(), State::ABORTED );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( child->terminations, 1 );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_Sequence, repeated_attachment_of_the_selected_child_does_not_abort_it )
{
   Sequence sequence;
   std::vector< int > calls;
   auto child = std::make_shared< RecordingBehavior >( 1, State::RUNNING, calls );
   sequence.attach( child );
   sequence.attach( child );

   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( child->getState(), State::RUNNING );
   EXPECT_EQ( child->terminations, 0 );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   sequence.abort();
   EXPECT_EQ( child->terminations, 1 );
}
