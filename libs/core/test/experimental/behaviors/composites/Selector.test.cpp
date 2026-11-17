#include <crimild/core/experimental/behaviors/composites/Selector.hpp>
#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::behaviors::Behavior;
   using crimild::experimental::behaviors::composites::Selector;
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

TEST( experimental_Selector, empty_selector_fails )
{
   Selector sequence;
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( sequence.getState(), State::FAILURE );
}

TEST( experimental_Selector, ticks_failed_children_in_order )
{
   Selector sequence;
   std::vector< int > calls;
   for ( int id : { 1, 2, 3 } ) {
      sequence.attach( std::make_shared< RecordingBehavior >( id, State::FAILURE, calls ) );
   }
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_Selector, success_stops_later_children )
{
   Selector sequence;
   std::vector< int > calls;
   sequence.attach( std::make_shared< RecordingBehavior >( 1, State::FAILURE, calls ) );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::SUCCESS, calls ) );
   auto later = std::make_shared< RecordingBehavior >( 3, State::FAILURE, calls );
   sequence.attach( later );

   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( sequence.getState(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_Selector, running_child_stops_later_children )
{
   Selector sequence;
   std::vector< int > calls;
   sequence.attach( std::make_shared< RecordingBehavior >( 1, State::FAILURE, calls ) );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls ) );
   // A later success must not override the running child's result.
   auto later = std::make_shared< RecordingBehavior >( 3, State::SUCCESS, calls );
   sequence.attach( later );

   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( sequence.getState(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( experimental_Selector, proceeds_when_running_child_fails )
{
   Selector sequence;
   std::vector< int > calls;
   auto child = std::make_shared< RecordingBehavior >( 1, State::RUNNING, calls );
   sequence.attach( child );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::FAILURE, calls ) );

   EXPECT_EQ( sequence.tick(), State::RUNNING );
   child->result = State::FAILURE;
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 1, 2 } ) );
}

TEST( experimental_Selector, resumes_running_child_without_reevaluating_failed_children )
{
   Selector sequence;
   std::vector< int > calls;
   auto condition = std::make_shared< RecordingBehavior >( 1, State::FAILURE, calls );
   auto action = std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls );
   sequence.attach( condition );
   sequence.attach( action );
   sequence.attach( std::make_shared< RecordingBehavior >( 3, State::FAILURE, calls ) );

   ASSERT_EQ( sequence.tick(), State::RUNNING );
   condition->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 2 } ) );
   EXPECT_EQ( action->initializations, 1 );
   EXPECT_EQ( action->terminations, 0 );

   action->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 2, 3 } ) );
   EXPECT_EQ( action->initializations, 1 );
   EXPECT_EQ( action->terminations, 1 );

   calls.clear();
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
}

TEST( experimental_Selector, defers_children_attached_during_execution_until_next_activation )
{
   Selector sequence;
   std::vector< int > calls;
   auto appended = std::make_shared< RecordingBehavior >( 3, State::FAILURE, calls );
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
      return State::FAILURE;
   };
   sequence.attach( mutator );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::FAILURE, calls ) );

   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( appended->getState(), State::READY );

   calls.clear();
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   std::vector< int > expected( sequence.getBehaviors().size(), 3 );
   expected[ 0 ] = 1;
   expected[ 1 ] = 2;
   EXPECT_EQ( calls, expected );
}

TEST( experimental_Selector, restarts_from_first_child_after_abort )
{
   Selector sequence;
   std::vector< int > calls;
   auto first = std::make_shared< RecordingBehavior >( 1, State::FAILURE, calls );
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

TEST( experimental_Selector, running_child_success_stops_and_next_activation_restarts )
{
   Selector sequence;
   std::vector< int > calls;
   auto running = std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls );
   auto later = std::make_shared< RecordingBehavior >( 3, State::FAILURE, calls );
   sequence.attach( std::make_shared< RecordingBehavior >( 1, State::FAILURE, calls ) );
   sequence.attach( running );
   sequence.attach( later );
   ASSERT_EQ( sequence.tick(), State::RUNNING );

   running->result = State::SUCCESS;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
   EXPECT_EQ( running->terminations, 1 );

   running->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( experimental_Selector, attachments_while_running_wait_until_next_activation )
{
   Selector sequence;
   std::vector< int > calls;
   auto running = std::make_shared< RecordingBehavior >( 1, State::RUNNING, calls );
   auto appended = std::make_shared< RecordingBehavior >( 2, State::FAILURE, calls );
   sequence.attach( running );
   ASSERT_EQ( sequence.tick(), State::RUNNING );
   sequence.attach( appended );

   running->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
   EXPECT_EQ( appended->getState(), State::READY );

   calls.clear();
   EXPECT_EQ( sequence.tick(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
}

TEST( experimental_Selector, abort_propagates_through_nested_sequences_and_decorators )
{
   Selector sequence;
   std::vector< int > calls;
   auto inner = std::make_shared< Selector >();
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

TEST( experimental_Selector, repeated_attachment_of_the_selected_child_does_not_abort_it )
{
   Selector sequence;
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
