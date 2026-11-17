#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {
   using crimild::experimental::behaviors::Behavior;
   using crimild::experimental::behaviors::composites::Sequence;
   using State = Behavior::State;

   class RecordingBehavior : public Behavior {
   public:
      RecordingBehavior( int id, State result, std::vector< int > &calls )
         : result( result ), m_id( id ), m_calls( calls )
      {
      }

      State onExecute( void ) override
      {
         m_calls.push_back( m_id );
         return result;
      }

      State result;

   private:
      int m_id;
      std::vector< int > &m_calls;
   };
}

TEST( ExperimentalSequence, empty_sequence_succeeds )
{
   Sequence sequence;
   EXPECT_EQ( sequence.execute(), State::SUCCESS );
   EXPECT_EQ( sequence.getState(), State::SUCCESS );
}

TEST( ExperimentalSequence, executes_successful_children_in_order )
{
   Sequence sequence;
   std::vector< int > calls;
   for ( int id : { 1, 2, 3 } ) {
      sequence.attach( std::make_shared< RecordingBehavior >( id, State::SUCCESS, calls ) );
   }
   EXPECT_EQ( sequence.execute(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2, 3 } ) );
}

TEST( ExperimentalSequence, failure_stops_later_children )
{
   Sequence sequence;
   std::vector< int > calls;
   sequence.attach( std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls ) );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::FAILURE, calls ) );
   auto later = std::make_shared< RecordingBehavior >( 3, State::SUCCESS, calls );
   sequence.attach( later );

   EXPECT_EQ( sequence.execute(), State::FAILURE );
   EXPECT_EQ( sequence.getState(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( ExperimentalSequence, running_child_stops_later_children )
{
   Sequence sequence;
   std::vector< int > calls;
   sequence.attach( std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls ) );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls ) );
   // A later failure must not override the running child's result.
   auto later = std::make_shared< RecordingBehavior >( 3, State::FAILURE, calls );
   sequence.attach( later );

   EXPECT_EQ( sequence.execute(), State::RUNNING );
   EXPECT_EQ( sequence.getState(), State::RUNNING );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 2 } ) );
   EXPECT_EQ( later->getState(), State::READY );
}

TEST( ExperimentalSequence, proceeds_when_running_child_succeeds )
{
   Sequence sequence;
   std::vector< int > calls;
   auto child = std::make_shared< RecordingBehavior >( 1, State::RUNNING, calls );
   sequence.attach( child );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::SUCCESS, calls ) );

   EXPECT_EQ( sequence.execute(), State::RUNNING );
   child->result = State::SUCCESS;
   EXPECT_EQ( sequence.execute(), State::SUCCESS );
   EXPECT_EQ( calls, ( std::vector< int > { 1, 1, 2 } ) );
}

TEST( ExperimentalSequence, reevaluates_earlier_children_on_each_execution )
{
   Sequence sequence;
   std::vector< int > calls;
   auto condition = std::make_shared< RecordingBehavior >( 1, State::SUCCESS, calls );
   sequence.attach( condition );
   sequence.attach( std::make_shared< RecordingBehavior >( 2, State::RUNNING, calls ) );

   EXPECT_EQ( sequence.execute(), State::RUNNING );
   condition->result = State::FAILURE;
   calls.clear();
   EXPECT_EQ( sequence.execute(), State::FAILURE );
   EXPECT_EQ( calls, ( std::vector< int > { 1 } ) );
}
