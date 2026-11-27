#include <crimild/core/experimental/nodes/events/Event.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::Blackboard;
   using crimild::experimental::Event;
   using crimild::experimental::behaviors::Behavior;

   class Action : public Behavior {
   public:
      int starts = 0;
      int updates = 0;
      State result = State::RUNNING;
      std::vector< Blackboard * > boards;
      std::vector< State > stops;
      std::function< void( Blackboard & ) > onUpdate;
      std::function< void() > onStop;

   protected:
      void initialize( Blackboard &board ) override
      {
         Behavior::initialize( board );
         ++starts;
      }

      State update( Blackboard &board ) override
      {
         ++updates;
         boards.push_back( &board );
         if ( onUpdate ) {
            onUpdate( board );
         }
         return result;
      }

      void terminate( State state ) override
      {
         stops.push_back( state );
         if ( onStop ) {
            onStop();
         }
      }
   };

}

TEST( experimental_Event, empty_events_are_noops_and_get_a_default_board )
{
   Event event;
   EXPECT_NE( event.getBlackboard(), nullptr );
   EXPECT_EQ( event.getBehavior(), nullptr );
   EXPECT_EQ( event.trigger(), Behavior::State::SUCCESS );
}

TEST( experimental_Event, resumes_a_running_activation_and_restarts_after_completion )
{
   Event event;
   auto action = std::make_shared< Action >();
   event.setBehavior( action );
   const auto board = event.getBlackboard();
   EXPECT_EQ( event.trigger(), Behavior::State::RUNNING );
   EXPECT_EQ( event.trigger(), Behavior::State::RUNNING );
   EXPECT_EQ( action->starts, 1 );
   EXPECT_EQ( action->updates, 2 );
   EXPECT_EQ( action->boards, ( std::vector< Blackboard * > { board.get(), board.get() } ) );
   action->result = Behavior::State::SUCCESS;
   EXPECT_EQ( event.trigger(), Behavior::State::SUCCESS );
   EXPECT_EQ( action->stops, ( std::vector { Behavior::State::SUCCESS } ) );
   event.trigger();
   EXPECT_EQ( action->starts, 2 );
}

TEST( experimental_Event, replacing_behavior_aborts_before_changing_the_member )
{
   Event event;
   const auto first = std::make_shared< Action >();
   const auto second = std::make_shared< Action >();
   event.setBehavior( first );
   event.trigger();
   first->onStop = [ & ] {
      EXPECT_EQ( event.getBehavior(), first );
   };
   event.setBehavior( second );
   EXPECT_EQ( first->stops, ( std::vector { Behavior::State::ABORTED } ) );
   EXPECT_EQ( event.getBehavior(), second );
   EXPECT_EQ( second->getState(), Behavior::State::READY );
   EXPECT_EQ( event.trigger(), Behavior::State::RUNNING );
}

TEST( experimental_Event, replacing_board_aborts_before_switching_execution_context )
{
   Event event;
   const auto action = std::make_shared< Action >();
   const auto first = event.getBlackboard();
   const auto second = std::make_shared< Blackboard >();
   event.setBehavior( action );
   event.trigger();
   action->onStop = [ & ] {
      EXPECT_EQ( event.getBlackboard(), first );
   };
   event.setBlackboard( second );
   EXPECT_EQ( action->stops, ( std::vector { Behavior::State::ABORTED } ) );
   EXPECT_EQ( event.getBlackboard(), second );
   event.trigger();
   EXPECT_EQ( action->starts, 2 );
   EXPECT_EQ( action->boards.back(), second.get() );
}

TEST( experimental_Event, assigning_the_same_handles_does_not_interrupt_running_work )
{
   Event event;
   const auto action = std::make_shared< Action >();
   event.setBehavior( action );
   event.trigger();
   event.setBehavior( action );
   event.setBlackboard( event.getBlackboard() );
   event.trigger();
   EXPECT_TRUE( action->stops.empty() );
   EXPECT_EQ( action->starts, 1 );
}

TEST( experimental_Event, clearing_members_cancels_work_and_missing_board_fails_without_initializing )
{
   Event event;
   const auto action = std::make_shared< Action >();
   event.setBehavior( action );
   event.trigger();
   event.setBehavior( nullptr );
   EXPECT_EQ( action->stops.size(), 1u );
   EXPECT_EQ( event.trigger(), Behavior::State::SUCCESS );
   event.setBehavior( action );
   event.trigger();
   event.setBlackboard( nullptr );
   EXPECT_EQ( action->stops.size(), 2u );
   EXPECT_EQ( event.trigger(), Behavior::State::FAILURE );
   EXPECT_EQ( action->starts, 2 );
   EXPECT_EQ( action->getState(), Behavior::State::ABORTED );
}

TEST( experimental_Event, disabled_events_do_not_start_their_behavior )
{
   Event event;
   const auto action = std::make_shared< Action >();
   event.setBehavior( action );
   event.setEnabled( false );
   EXPECT_EQ( event.trigger(), Behavior::State::SUCCESS );
   EXPECT_EQ( action->getState(), Behavior::State::READY );
   EXPECT_EQ( action->updates, 0 );
}

TEST( experimental_Event, pins_replaced_behavior_until_its_tick_returns )
{
   Event event;
   auto action = std::make_shared< Action >();
   const std::weak_ptr< Action > lifetime = action;
   const auto replacement = std::make_shared< Action >();
   bool stayedAlive = false;
   action->onUpdate = [ & ]( Blackboard & ) {
      action.reset();
      event.setBehavior( replacement );
      stayedAlive = !lifetime.expired();
   };
   event.setBehavior( action );
   EXPECT_EQ( event.trigger(), Behavior::State::ABORTED );
   EXPECT_TRUE( stayedAlive );
   EXPECT_TRUE( lifetime.expired() );
   EXPECT_EQ( replacement->updates, 0 );
}

TEST( experimental_Event, pins_the_original_board_across_callback_replacement )
{
   Event event;
   const auto action = std::make_shared< Action >();
   const auto replacement = std::make_shared< Blackboard >();
   const std::weak_ptr< Blackboard > lifetime = event.getBlackboard();
   bool stayedAlive = false;
   action->onUpdate = [ & ]( Blackboard &board ) {
      event.setBlackboard( replacement );
      stayedAlive = !lifetime.expired();
      board.set( "callback.finished", true );
   };
   event.setBehavior( action );
   EXPECT_EQ( event.trigger(), Behavior::State::ABORTED );
   EXPECT_TRUE( stayedAlive );
   EXPECT_TRUE( lifetime.expired() );
   EXPECT_FALSE( replacement->contains( "callback.finished" ) );
}

TEST( experimental_Event, cleanup_callbacks_can_choose_a_different_configuration )
{
   Event event;
   const auto action = std::make_shared< Action >();
   const auto requested = std::make_shared< Action >();
   const auto chosen = std::make_shared< Action >();
   event.setBehavior( action );
   event.trigger();
   action->onStop = [ & ] {
      event.setBehavior( chosen );
   };
   event.setBehavior( requested );
   EXPECT_EQ( event.getBehavior(), chosen );
   EXPECT_EQ( action->stops.size(), 1u );
}

TEST( experimental_Event, cleanup_can_replace_board_without_an_outer_setter_overwriting_it )
{
   Event event;
   const auto action = std::make_shared< Action >();
   const auto requested = std::make_shared< Blackboard >();
   const auto chosen = std::make_shared< Blackboard >();
   event.setBehavior( action );
   event.trigger();
   action->onStop = [ & ] {
      event.setBlackboard( chosen );
   };
   event.setBlackboard( requested );
   EXPECT_EQ( event.getBlackboard(), chosen );
   EXPECT_EQ( action->stops.size(), 1u );
}

TEST( experimental_Event, separate_behaviors_exchange_information_through_a_shared_board )
{
   Event writer;
   Event reader;
   reader.setBlackboard( writer.getBlackboard() );
   const auto publish = std::make_shared< Action >();
   const auto consume = std::make_shared< Action >();
   publish->result = Behavior::State::SUCCESS;
   consume->result = Behavior::State::SUCCESS;
   publish->onUpdate = []( Blackboard &board ) {
      board.set( "count", uint32_t { 7 } );
   };
   consume->onUpdate = []( Blackboard &board ) {
      EXPECT_EQ( board.get< uint32_t >( "count" ), 7u );
   };
   writer.setBehavior( publish );
   reader.setBehavior( consume );
   writer.trigger();
   reader.trigger();
   EXPECT_EQ( publish->updates, 1 );
   EXPECT_EQ( consume->updates, 1 );
}

TEST( experimental_Event, a_cleanup_callback_can_restart_the_current_activation_without_switching_its_board )
{
   Event event;
   const auto action = std::make_shared< Action >();
   const auto original = event.getBlackboard();
   event.setBehavior( action );
   event.trigger();
   action->onStop = [ & ] {
      event.trigger();
   };
   event.setBlackboard( std::make_shared< Blackboard >() );
   EXPECT_EQ( event.getBlackboard(), original );
   EXPECT_EQ( action->getState(), Behavior::State::RUNNING );
   EXPECT_EQ( action->starts, 2 );
   EXPECT_EQ( action->boards.back(), original.get() );
   event.setBehavior( std::make_shared< Action >() );
   EXPECT_EQ( event.getBehavior(), action );
   EXPECT_EQ( action->starts, 3 );
}
