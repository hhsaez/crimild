#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <crimild/core/experimental/behaviors/composites/ActiveSelector.hpp>
#include <crimild/core/experimental/behaviors/composites/ActiveSequence.hpp>
#include <crimild/core/experimental/behaviors/composites/Parallel.hpp>
#include <crimild/core/experimental/behaviors/composites/Selector.hpp>
#include <crimild/core/experimental/behaviors/composites/Sequence.hpp>
#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

   using crimild::experimental::Blackboard;
   using crimild::experimental::behaviors::Behavior;

   class TestBehavior : public Behavior {
   public:
      State result = State::RUNNING;
      int updates = 0;

   protected:
      State update( Blackboard &blackboard ) override
      {
         ++updates;
         return result;
      }
   };

   class LifecycleBehavior : public TestBehavior {
   public:
      std::vector< std::string > calls;
      std::vector< State > terminationStates;
      std::function< void() > onInitialize;
      std::function< void() > onUpdate;
      std::function< void( State ) > onTerminate;

   protected:
      void initialize( Blackboard &blackboard ) override
      {
         TestBehavior::initialize( blackboard );
         calls.push_back( "initialize" );
         if ( onInitialize ) {
            onInitialize();
         }
      }

      State update( Blackboard &blackboard ) override
      {
         calls.push_back( "update" );
         if ( onUpdate ) {
            onUpdate();
         }
         return TestBehavior::update( blackboard );
      }

      void terminate( State state ) override
      {
         calls.push_back( "terminate" );
         terminationStates.push_back( state );
         EXPECT_EQ( state, getState() );
         if ( onTerminate ) {
            onTerminate( state );
         }
      }
   };

}

TEST( experimental_Behavior, supports_naming )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Behavior behavior;
   EXPECT_TRUE( behavior.getName().empty() );

   behavior.setName( "Patrol" );
   EXPECT_EQ( behavior.getName(), "Patrol" );

   behavior.setName( "Chase" );
   EXPECT_EQ( behavior.getName(), "Chase" );
}

TEST( experimental_Behavior, starts_ready_without_ticking )
{
   const auto blackboard = std::make_shared< Blackboard >();
   TestBehavior behavior;
   EXPECT_EQ( behavior.getState(), Behavior::State::READY );
   EXPECT_EQ( behavior.updates, 0 );
}

TEST( experimental_Behavior, default_tick_succeeds )
{
   const auto blackboard = std::make_shared< Blackboard >();
   Behavior behavior;
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
}

TEST( experimental_Behavior, stores_each_tick_result )
{
   const auto blackboard = std::make_shared< Blackboard >();
   TestBehavior behavior;
   for ( const auto state : { Behavior::State::RUNNING, Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      behavior.result = state;
      EXPECT_EQ( behavior.tick( *blackboard ), state );
      EXPECT_EQ( behavior.getState(), state );
   }
   EXPECT_EQ( behavior.updates, 3 );
}

TEST( experimental_Behavior, keeps_ticking_running_behavior_until_success )
{
   const auto blackboard = std::make_shared< Blackboard >();
   TestBehavior behavior;
   EXPECT_EQ( behavior.getState(), Behavior::State::READY );

   // update( blackboard ) requests another tick until the action reports completion.
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.updates, 2 );

   behavior.result = Behavior::State::SUCCESS;
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.updates, 3 );
}

TEST( experimental_Behavior, initializes_before_the_first_update )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   EXPECT_TRUE( behavior.calls.empty() );

   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update" } ) );
   EXPECT_TRUE( behavior.terminationStates.empty() );
}

TEST( experimental_Behavior, running_ticks_do_not_repeat_initialization_or_terminate )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   for ( int tick = 0; tick < 3; ++tick ) {
      EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
   }

   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "update", "update" } ) );
   EXPECT_TRUE( behavior.terminationStates.empty() );
}

TEST( experimental_Behavior, immediate_completion_terminates_with_the_stored_final_state )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      LifecycleBehavior behavior;
      behavior.result = result;

      EXPECT_EQ( behavior.tick( *blackboard ), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );
   }
}

TEST( experimental_Behavior, running_activation_terminates_once_when_it_completes )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      LifecycleBehavior behavior;
      ASSERT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );

      behavior.result = result;
      EXPECT_EQ( behavior.tick( *blackboard ), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );
   }
}

TEST( experimental_Behavior, ticking_after_completion_starts_a_new_activation )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      LifecycleBehavior behavior;
      behavior.result = result;
      ASSERT_EQ( behavior.tick( *blackboard ), result );

      behavior.result = Behavior::State::RUNNING;
      EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );

      behavior.result = result;
      EXPECT_EQ( behavior.tick( *blackboard ), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result, result } ) );
   }
}

TEST( experimental_Behavior, abort_terminates_a_running_activation_once_without_updating )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   ASSERT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );

   behavior.abort();
   behavior.abort();

   EXPECT_EQ( behavior.getState(), Behavior::State::ABORTED );
   EXPECT_EQ( behavior.updates, 1 );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate" } ) );
   EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}

TEST( experimental_Behavior, abort_before_activation_does_nothing )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   behavior.abort();

   EXPECT_EQ( behavior.getState(), Behavior::State::READY );
   EXPECT_TRUE( behavior.calls.empty() );
}

TEST( experimental_Behavior, abort_after_completion_preserves_the_result_without_terminating_again )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      SCOPED_TRACE( static_cast< int >( result ) );
      LifecycleBehavior behavior;
      behavior.result = result;
      ASSERT_EQ( behavior.tick( *blackboard ), result );

      behavior.abort();

      EXPECT_EQ( behavior.getState(), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );
   }
}

TEST( experimental_Behavior, ticking_after_abort_starts_a_new_activation )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   ASSERT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
   behavior.abort();

   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update" } ) );
   EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}

TEST( experimental_Behavior, rejects_lifecycle_states_returned_by_update )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( const auto state : { Behavior::State::READY, Behavior::State::ABORTED } ) {
      TestBehavior behavior;
      behavior.result = state;
      EXPECT_DEBUG_DEATH( behavior.tick( *blackboard ), "update\\(\\) must return SUCCESS, FAILURE, or RUNNING" );
   }
}

TEST( experimental_Behavior, abort_during_initialize_skips_update_and_terminates_once )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   behavior.onInitialize = [ & ] {
      behavior.abort();
   };
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::ABORTED );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "terminate" } ) );
   EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}

TEST( experimental_Behavior, abort_during_update_preserves_cancellation_on_first_and_later_ticks )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( const bool firstTick : { true, false } ) {
      for ( const auto result : { Behavior::State::SUCCESS, Behavior::State::FAILURE, Behavior::State::RUNNING } ) {
         LifecycleBehavior behavior;
         if ( !firstTick ) {
            ASSERT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
         }
         behavior.result = result;
         behavior.onUpdate = [ & ] {
            behavior.abort();
            behavior.abort();
         };
         EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::ABORTED );
         EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
         behavior.onUpdate = nullptr;
         behavior.result = Behavior::State::SUCCESS;
         EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::SUCCESS );
         EXPECT_EQ( behavior.terminationStates.size(), 2 );
      }
   }
}

TEST( experimental_Behavior, termination_exposes_the_final_state_and_recursive_abort_does_not_repeat_cleanup )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( const auto reason : { Behavior::State::SUCCESS, Behavior::State::FAILURE, Behavior::State::ABORTED } ) {
      LifecycleBehavior behavior;
      behavior.onTerminate = [ & ]( Behavior::State state ) {
         EXPECT_EQ( state, reason );
         ASSERT_EQ( behavior.getState(), reason );
         behavior.abort();
         behavior.abort();
      };
      ASSERT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
      if ( reason == Behavior::State::ABORTED ) {
         behavior.abort();
      } else {
         behavior.result = reason;
         EXPECT_EQ( behavior.tick( *blackboard ), reason );
      }
      EXPECT_EQ( behavior.getState(), reason );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { reason } ) );
   }
}

TEST( experimental_Behavior, initializer_owns_the_running_transition_on_first_and_restarted_activations )
{
   const auto blackboard = std::make_shared< Blackboard >();
   class Initializer : public TestBehavior {
   public:
      std::vector< State > entryStates;

   protected:
      void initialize( Blackboard &blackboard ) override
      {
         const auto previous = getState();
         TestBehavior::initialize( blackboard );
         entryStates.push_back( previous );
         EXPECT_EQ( getState(), State::RUNNING );
      }
   };
   Initializer behavior;
   behavior.result = Behavior::State::SUCCESS;
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::SUCCESS );
   behavior.result = Behavior::State::FAILURE;
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::FAILURE );
   behavior.result = Behavior::State::RUNNING;
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::RUNNING );
   behavior.abort();
   behavior.result = Behavior::State::SUCCESS;
   EXPECT_EQ( behavior.tick( *blackboard ), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.entryStates, ( std::vector< Behavior::State > {
                                       Behavior::State::READY,
                                       Behavior::State::SUCCESS,
                                       Behavior::State::FAILURE,
                                       Behavior::State::ABORTED,
                                    } ) );
}

TEST( experimental_Behavior, initialization_must_chain_to_the_base_implementation )
{
   const auto blackboard = std::make_shared< Blackboard >();
   class MissingBase : public TestBehavior {
   protected:
      void initialize( Blackboard &blackboard ) override { }
   };
   MissingBase behavior;
   EXPECT_DEBUG_DEATH( behavior.tick( *blackboard ), "must call Behavior::initialize" );
}

namespace {

   class BoardAction : public Behavior {
   public:
      State result = State::RUNNING;
      std::vector< Blackboard * > seen;
      std::function< void( Blackboard & ) > onTick;
      std::function< void( Blackboard & ) > onStop;
      std::shared_ptr< Blackboard > retained;
      std::shared_ptr< crimild::Entity > reservation;
      std::vector< State > terminationStates;

   private:
      std::weak_ptr< Blackboard > cleanupBoard;

   protected:
      void initialize( Blackboard &board ) override
      {
         Behavior::initialize( board );
         cleanupBoard = retain( &board );
         reservation = board.get< std::shared_ptr< crimild::Entity > >( "reservation" ).value_or( nullptr );
         seen.push_back( &board );
         board.set( "started", true );
      }

      State update( Blackboard &board ) override
      {
         seen.push_back( &board );
         EXPECT_EQ( board.get< bool >( "started" ), true );
         if ( onTick ) {
            onTick( board );
         }
         return result;
      }

      void terminate( State state ) override
      {
         EXPECT_EQ( state, getState() );
         terminationStates.push_back( state );
         reservation.reset();
         // This subclass opts into board-dependent bookkeeping; cleanup itself
         // still runs if the execution board has already been destroyed.
         if ( auto board = cleanupBoard.lock() ) {
            seen.push_back( board.get() );
            board->set( "stopped", true );
            if ( onStop ) {
               onStop( *board );
            }
         }
      }
   };

}

TEST( experimental_Behavior, execution_hooks_share_the_board_and_termination_receives_the_final_state )
{
   for ( const auto finish : { Behavior::State::SUCCESS, Behavior::State::FAILURE, Behavior::State::ABORTED } ) {
      const auto board = std::make_shared< Blackboard >();
      BoardAction action;
      EXPECT_EQ( action.tick( *board ), Behavior::State::RUNNING );
      EXPECT_FALSE( board->contains( "stopped" ) );
      if ( finish == Behavior::State::ABORTED ) {
         action.abort();
         action.abort();
      } else {
         action.result = finish;
         EXPECT_EQ( action.tick( *board ), finish );
      }
      EXPECT_EQ( board->get< bool >( "stopped" ), true );
      EXPECT_EQ( action.terminationStates, ( std::vector< Behavior::State > { finish } ) );
      EXPECT_EQ( action.seen.size(), finish == Behavior::State::ABORTED ? 3u : 4u );
      for ( const auto seen : action.seen ) {
         EXPECT_EQ( seen, board.get() );
      }
   }
}

TEST( experimental_Behavior, caller_locks_the_board_for_the_root_tick )
{
   crimild::experimental::behaviors::composites::Sequence root;
   auto action = std::make_shared< BoardAction >();
   root.attach( action );
   auto board = std::make_shared< Blackboard >();
   std::weak_ptr< Blackboard > lifetime = board;
   action->onTick = [ &board, &lifetime ]( auto &passed ) {
      board.reset();
      EXPECT_FALSE( lifetime.expired() );
      EXPECT_EQ( lifetime.use_count(), 1 );
      passed.set( "available", true );
   };
   if ( auto locked = lifetime.lock() ) {
      EXPECT_EQ( root.tick( *locked ), Behavior::State::RUNNING );
      EXPECT_EQ( locked->get< bool >( "available" ), true );
   } else {
      FAIL() << "The execution board must be available before ticking the root";
   }
   EXPECT_TRUE( lifetime.expired() );
   root.abort();
   EXPECT_EQ( action->terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}

TEST( experimental_Behavior, subclass_can_explicitly_retain_the_board )
{
   BoardAction action;
   auto board = std::make_shared< Blackboard >();
   std::weak_ptr< Blackboard > lifetime = board;
   action.onTick = [ &action ]( auto &passed ) {
      action.retained = retain( &passed );
   };
   EXPECT_EQ( action.tick( *board ), Behavior::State::RUNNING );
   board.reset();
   EXPECT_FALSE( lifetime.expired() );
   action.onStop = [ &action ]( auto &passed ) {
      action.retained.reset();
      EXPECT_EQ( passed.template get< bool >( "started" ), true );
   };
   action.abort();
   EXPECT_TRUE( lifetime.expired() );
   EXPECT_EQ( action.getState(), Behavior::State::ABORTED );
}

TEST( experimental_Behavior, execution_can_borrow_a_stack_board_without_shared_ownership )
{
   Blackboard board;
   LifecycleBehavior action;
   action.onInitialize = [ &board ] {
      board.set( "started", true );
   };
   action.onUpdate = [ &board ] {
      EXPECT_EQ( board.get< bool >( "started" ), true );
   };
   EXPECT_EQ( action.tick( board ), Behavior::State::RUNNING );
   EXPECT_TRUE( board.weak_from_this().expired() );
   action.abort();
   EXPECT_EQ( action.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}

namespace {

   namespace composites = crimild::experimental::behaviors::composites;

   class ForwardingDecorator : public crimild::experimental::behaviors::decorators::Decorator {
   public:
      ForwardingDecorator() = default;

   protected:
      State update( Blackboard &board ) override
      {
         return getBehavior()->tick( board );
      }
   };

}

TEST( experimental_Behavior, every_composite_forwards_the_board_across_ticks_and_cancellation )
{
   const std::vector< std::shared_ptr< composites::Composite > > roots {
      std::make_shared< composites::Sequence >(),
      std::make_shared< composites::Selector >(),
      std::make_shared< composites::Parallel >(),
      std::make_shared< composites::ActiveSequence >(),
      std::make_shared< composites::ActiveSelector >(),
   };
   for ( const auto &root : roots ) {
      const auto board = std::make_shared< Blackboard >();
      auto action = std::make_shared< BoardAction >();
      action->onTick = []( auto &passed ) {
         EXPECT_EQ( passed.weak_from_this().use_count(), 1 );
         passed.set( "ticks", passed.template get< uint32_t >( "ticks" ).value_or( 0 ) + 1 );
      };
      auto decorator = std::make_shared< ForwardingDecorator >();
      decorator->attach( action );
      root->attach( decorator );
      EXPECT_EQ( root->tick( *board ), Behavior::State::RUNNING );
      EXPECT_EQ( root->tick( *board ), Behavior::State::RUNNING );
      EXPECT_EQ( board->get< uint32_t >( "ticks" ), 2u );
      root->abort();
      EXPECT_EQ( board->get< bool >( "stopped" ), true );
      ASSERT_EQ( action->seen.size(), 4u );
      for ( auto seen : action->seen ) {
         EXPECT_EQ( seen, board.get() );
      }
   }
}

TEST( experimental_Behavior, sequence_siblings_share_a_target_and_preserve_data_across_ticks )
{
   const auto board = std::make_shared< Blackboard >();
   const auto target = std::make_shared< crimild::Entity >();
   auto findTarget = std::make_shared< BoardAction >();
   findTarget->result = Behavior::State::SUCCESS;
   findTarget->onTick = [ &target ]( auto &passed ) {
      passed.set( "target", target );
   };
   auto shoot = std::make_shared< BoardAction >();
   shoot->onTick = [ &target ]( auto &passed ) {
      EXPECT_EQ( passed.template get< std::shared_ptr< crimild::Entity > >( "target" ), target );
      passed.set( "shots", passed.template get< uint32_t >( "shots" ).value_or( 0 ) + 1 );
   };
   composites::Sequence root;
   root.attach( findTarget );
   root.attach( shoot );
   EXPECT_EQ( root.tick( *board ), Behavior::State::RUNNING );
   EXPECT_EQ( root.tick( *board ), Behavior::State::RUNNING );
   EXPECT_EQ( findTarget->seen.size(), 3u ); // initialize, update, terminate only once
   EXPECT_EQ( board->get< uint32_t >( "shots" ), 2u );
   root.abort();
   EXPECT_EQ( board->get< std::shared_ptr< crimild::Entity > >( "target" ), target );
}

TEST( experimental_Behavior, running_tree_edits_cancel_without_a_board )
{
   const auto check = []( auto &root ) {
      const auto board = std::make_shared< Blackboard >();
      const auto child = std::make_shared< BoardAction >();
      const auto replacement = std::make_shared< BoardAction >();
      root.attach( child );
      ASSERT_EQ( root.tick( *board ), Behavior::State::RUNNING );
      // No-op edits do not need a board or interrupt execution.
      root.attach( child );
      EXPECT_EQ( root.getState(), Behavior::State::RUNNING );
      // Real edits cancel immediately; cleanup uses captured activation data.
      root.attach( replacement );
      EXPECT_EQ( root.getState(), Behavior::State::ABORTED );
      EXPECT_EQ( child->getState(), Behavior::State::ABORTED );
      EXPECT_EQ( board->get< bool >( "stopped" ), true );
      EXPECT_EQ( child->seen.back(), board.get() );
      EXPECT_TRUE( replacement->hasParent() );
   };
   composites::Sequence sequence;
   check( sequence );
   ForwardingDecorator decorator;
   check( decorator );
}

TEST( experimental_Behavior, automatic_reparenting_between_distinct_boards_cleans_up_each_activation )
{
   const auto sourceBoard = std::make_shared< Blackboard >();
   const auto destinationBoard = std::make_shared< Blackboard >();
   composites::Sequence source;
   ForwardingDecorator destination;
   const auto child = std::make_shared< BoardAction >();
   const auto previous = std::make_shared< BoardAction >();
   source.attach( child );
   destination.attach( previous );
   ASSERT_EQ( source.tick( *sourceBoard ), Behavior::State::RUNNING );
   ASSERT_EQ( destination.tick( *destinationBoard ), Behavior::State::RUNNING );
   destination.attach( child );
   EXPECT_EQ( child->seen.back(), sourceBoard.get() );
   EXPECT_EQ( previous->seen.back(), destinationBoard.get() );
   EXPECT_EQ( source.getState(), Behavior::State::ABORTED );
   EXPECT_EQ( destination.getState(), Behavior::State::ABORTED );
   EXPECT_EQ( sourceBoard->get< bool >( "stopped" ), true );
   EXPECT_EQ( destinationBoard->get< bool >( "stopped" ), true );
   EXPECT_EQ( destination.tick( *destinationBoard ), Behavior::State::RUNNING );
   EXPECT_EQ( child->seen.back(), destinationBoard.get() );
   destination.abort();
}

TEST( experimental_Behavior, abort_cleans_up_captured_resources_after_the_board_is_destroyed )
{
   BoardAction action;
   auto board = std::make_shared< Blackboard >();
   auto reservation = std::make_shared< crimild::Entity >();
   std::weak_ptr< crimild::Entity > resourceLifetime = reservation;
   std::weak_ptr< Blackboard > boardLifetime = board;
   board->set( "reservation", reservation );
   EXPECT_EQ( action.tick( *board ), Behavior::State::RUNNING );
   reservation.reset();
   board.reset();
   EXPECT_TRUE( boardLifetime.expired() );
   EXPECT_FALSE( resourceLifetime.expired() );
   action.abort();
   action.abort();
   EXPECT_TRUE( resourceLifetime.expired() );
   EXPECT_EQ( action.getState(), Behavior::State::ABORTED );
   EXPECT_EQ( action.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}
