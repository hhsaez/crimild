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
      State update( std::shared_ptr< Blackboard > const &blackboard ) override
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

   protected:
      void initialize( std::shared_ptr< Blackboard > const &blackboard ) override
      {
         calls.push_back( "initialize" );
         if ( onInitialize ) {
            onInitialize();
         }
      }

      State update( std::shared_ptr< Blackboard > const &blackboard ) override
      {
         calls.push_back( "update" );
         if ( onUpdate ) {
            onUpdate();
         }
         return TestBehavior::update( blackboard );
      }

      void terminate( std::shared_ptr< Blackboard > const &blackboard ) override
      {
         calls.push_back( "terminate" );
         terminationStates.push_back( getState() );
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
   EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
}

TEST( experimental_Behavior, stores_each_tick_result )
{
   const auto blackboard = std::make_shared< Blackboard >();
   TestBehavior behavior;
   for ( const auto state : { Behavior::State::RUNNING, Behavior::State::SUCCESS, Behavior::State::FAILURE } ) {
      behavior.result = state;
      EXPECT_EQ( behavior.tick( blackboard ), state );
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
   EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.updates, 2 );

   behavior.result = Behavior::State::SUCCESS;
   EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.getState(), Behavior::State::SUCCESS );
   EXPECT_EQ( behavior.updates, 3 );
}

TEST( experimental_Behavior, initializes_before_the_first_update )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   EXPECT_TRUE( behavior.calls.empty() );

   EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.getState(), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update" } ) );
   EXPECT_TRUE( behavior.terminationStates.empty() );
}

TEST( experimental_Behavior, running_ticks_do_not_repeat_initialization_or_terminate )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   for ( int tick = 0; tick < 3; ++tick ) {
      EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );
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

      EXPECT_EQ( behavior.tick( blackboard ), result );
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
      ASSERT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );

      behavior.result = result;
      EXPECT_EQ( behavior.tick( blackboard ), result );
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
      ASSERT_EQ( behavior.tick( blackboard ), result );

      behavior.result = Behavior::State::RUNNING;
      EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );

      behavior.result = result;
      EXPECT_EQ( behavior.tick( blackboard ), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result, result } ) );
   }
}

TEST( experimental_Behavior, abort_terminates_a_running_activation_once_without_updating )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   ASSERT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );

   behavior.abort( blackboard );
   behavior.abort( blackboard );

   EXPECT_EQ( behavior.getState(), Behavior::State::ABORTED );
   EXPECT_EQ( behavior.updates, 1 );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate" } ) );
   EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}

TEST( experimental_Behavior, abort_before_activation_does_nothing )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   behavior.abort( blackboard );

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
      ASSERT_EQ( behavior.tick( blackboard ), result );

      behavior.abort( blackboard );

      EXPECT_EQ( behavior.getState(), result );
      EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate" } ) );
      EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { result } ) );
   }
}

TEST( experimental_Behavior, ticking_after_abort_starts_a_new_activation )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   ASSERT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );
   behavior.abort( blackboard );

   EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );
   EXPECT_EQ( behavior.calls, ( std::vector< std::string > { "initialize", "update", "terminate", "initialize", "update" } ) );
   EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
}

TEST( experimental_Behavior, rejects_lifecycle_states_returned_by_update )
{
   const auto blackboard = std::make_shared< Blackboard >();
   for ( const auto state : { Behavior::State::READY, Behavior::State::ABORTED } ) {
      TestBehavior behavior;
      behavior.result = state;
      EXPECT_DEBUG_DEATH( behavior.tick( blackboard ), "update\\(\\) must return SUCCESS, FAILURE, or RUNNING" );
   }
}

TEST( experimental_Behavior, abort_during_initialize_skips_update_and_terminates_once )
{
   const auto blackboard = std::make_shared< Blackboard >();
   LifecycleBehavior behavior;
   behavior.onInitialize = [ & ] {
      behavior.abort( blackboard );
   };
   EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::ABORTED );
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
            ASSERT_EQ( behavior.tick( blackboard ), Behavior::State::RUNNING );
         }
         behavior.result = result;
         behavior.onUpdate = [ & ] {
            behavior.abort( blackboard );
            behavior.abort( blackboard );
         };
         EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::ABORTED );
         EXPECT_EQ( behavior.terminationStates, ( std::vector< Behavior::State > { Behavior::State::ABORTED } ) );
         behavior.onUpdate = nullptr;
         behavior.result = Behavior::State::SUCCESS;
         EXPECT_EQ( behavior.tick( blackboard ), Behavior::State::SUCCESS );
         EXPECT_EQ( behavior.terminationStates.size(), 2 );
      }
   }
}

namespace {

   class BoardAction : public Behavior {
   public:
      State result = State::RUNNING;
      std::vector< Blackboard * > seen;
      std::function< void( std::shared_ptr< Blackboard > const & ) > onTick;
      std::function< void( std::shared_ptr< Blackboard > const & ) > onStop;
      std::shared_ptr< Blackboard > retained;

   protected:
      void initialize( std::shared_ptr< Blackboard > const &board ) override
      {
         seen.push_back( board.get() );
         board->set( "started", true );
      }

      State update( std::shared_ptr< Blackboard > const &board ) override
      {
         seen.push_back( board.get() );
         EXPECT_EQ( board->get< bool >( "started" ), true );
         if ( onTick ) {
            onTick( board );
         }
         return result;
      }

      void terminate( std::shared_ptr< Blackboard > const &board ) override
      {
         seen.push_back( board.get() );
         board->set( "stopped", true );
         if ( onStop ) {
            onStop( board );
         }
      }
   };

}

TEST( experimental_Behavior, lifecycle_hooks_share_the_board_on_success_failure_and_abort )
{
   for ( const auto finish : { Behavior::State::SUCCESS, Behavior::State::FAILURE, Behavior::State::ABORTED } ) {
      const auto board = std::make_shared< Blackboard >();
      BoardAction action;
      EXPECT_EQ( action.tick( board ), Behavior::State::RUNNING );
      EXPECT_FALSE( board->contains( "stopped" ) );
      if ( finish == Behavior::State::ABORTED ) {
         action.abort( board );
         action.abort( board );
      } else {
         action.result = finish;
         EXPECT_EQ( action.tick( board ), finish );
      }
      EXPECT_EQ( board->get< bool >( "stopped" ), true );
      EXPECT_EQ( action.seen.size(), finish == Behavior::State::ABORTED ? 3u : 4u );
      for ( const auto seen : action.seen ) {
         EXPECT_EQ( seen, board.get() );
      }
   }
}

TEST( experimental_Behavior, only_retains_the_argument_during_the_call )
{
   BoardAction action;
   auto board = std::make_shared< Blackboard >();
   std::weak_ptr< Blackboard > lifetime = board;
   action.onTick = [ &board, &lifetime ]( auto const &passed ) {
      board.reset();
      EXPECT_FALSE( lifetime.expired() );
      passed->set( "available", true );
   };
   EXPECT_EQ( action.tick( board ), Behavior::State::RUNNING );
   EXPECT_TRUE( lifetime.expired() );
}

TEST( experimental_Behavior, subclass_can_explicitly_retain_the_board )
{
   BoardAction action;
   auto board = std::make_shared< Blackboard >();
   std::weak_ptr< Blackboard > lifetime = board;
   action.onTick = [ &action ]( auto const &passed ) {
      action.retained = passed;
   };
   EXPECT_EQ( action.tick( board ), Behavior::State::RUNNING );
   board.reset();
   EXPECT_FALSE( lifetime.expired() );
   action.onStop = [ &action ]( auto const &passed ) {
      action.retained.reset();
      EXPECT_EQ( passed->template get< bool >( "started" ), true );
   };
   action.abort( action.retained );
   EXPECT_TRUE( lifetime.expired() );
   EXPECT_EQ( action.getState(), Behavior::State::ABORTED );
}

TEST( experimental_Behavior, rejects_null_boards_without_changing_activation_state )
{
   BoardAction action;
   EXPECT_EQ( action.tick( nullptr ), Behavior::State::FAILURE );
   EXPECT_EQ( action.getState(), Behavior::State::READY );
   EXPECT_TRUE( action.seen.empty() );
   const auto board = std::make_shared< Blackboard >();
   action.tick( board );
   action.abort( nullptr );
   EXPECT_EQ( action.getState(), Behavior::State::RUNNING );
   EXPECT_FALSE( board->contains( "stopped" ) );
   action.abort( board );
   EXPECT_EQ( board->get< bool >( "stopped" ), true );
}

namespace {

   namespace composites = crimild::experimental::behaviors::composites;

   class ForwardingDecorator : public crimild::experimental::behaviors::decorators::Decorator {
   public:
      ForwardingDecorator() = default;

   protected:
      State update( std::shared_ptr< Blackboard > const &board ) override
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
      action->onTick = []( auto const &passed ) {
         passed->set( "ticks", passed->template get< uint32_t >( "ticks" ).value_or( 0 ) + 1 );
      };
      root->attach( action );
      EXPECT_EQ( root->tick( board ), Behavior::State::RUNNING );
      EXPECT_EQ( root->tick( board ), Behavior::State::RUNNING );
      EXPECT_EQ( board->get< uint32_t >( "ticks" ), 2u );
      root->abort( board );
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
   findTarget->onTick = [ &target ]( auto const &passed ) {
      passed->set( "target", target );
   };
   auto shoot = std::make_shared< BoardAction >();
   shoot->onTick = [ &target ]( auto const &passed ) {
      EXPECT_EQ( passed->template get< std::shared_ptr< crimild::Entity > >( "target" ), target );
      passed->set( "shots", passed->template get< uint32_t >( "shots" ).value_or( 0 ) + 1 );
   };
   composites::Sequence root;
   root.attach( findTarget );
   root.attach( shoot );
   EXPECT_EQ( root.tick( board ), Behavior::State::RUNNING );
   EXPECT_EQ( root.tick( board ), Behavior::State::RUNNING );
   EXPECT_EQ( findTarget->seen.size(), 3u ); // initialize, update, terminate only once
   EXPECT_EQ( board->get< uint32_t >( "shots" ), 2u );
   root.abort( board );
   EXPECT_EQ( board->get< std::shared_ptr< crimild::Entity > >( "target" ), target );
}

TEST( experimental_Behavior, running_tree_edits_require_a_board_and_pass_it_to_cleanup )
{
   const auto check = []( auto &root ) {
      const auto board = std::make_shared< Blackboard >();
      const auto child = std::make_shared< BoardAction >();
      const auto replacement = std::make_shared< BoardAction >();
      root.attach( child );
      ASSERT_EQ( root.tick( board ), Behavior::State::RUNNING );
      // No-op edits do not need a board or interrupt execution.
      root.attach( child );
      EXPECT_EQ( root.getState(), Behavior::State::RUNNING );
      // Real edits without the activation's board leave state and ownership intact.
      root.attach( replacement );
      root.detach( child );
      child->detachFromParent();
      EXPECT_TRUE( child->hasParent() );
      EXPECT_FALSE( replacement->hasParent() );
      EXPECT_EQ( root.getState(), Behavior::State::RUNNING );
      EXPECT_FALSE( board->contains( "stopped" ) );
      root.attach( replacement, board );
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

TEST( experimental_Behavior, explicit_reparenting_between_distinct_boards_cleans_up_with_each_owners_board )
{
   const auto sourceBoard = std::make_shared< Blackboard >();
   const auto destinationBoard = std::make_shared< Blackboard >();
   composites::Sequence source;
   ForwardingDecorator destination;
   const auto child = std::make_shared< BoardAction >();
   const auto previous = std::make_shared< BoardAction >();
   source.attach( child );
   destination.attach( previous );
   ASSERT_EQ( source.tick( sourceBoard ), Behavior::State::RUNNING );
   ASSERT_EQ( destination.tick( destinationBoard ), Behavior::State::RUNNING );
   child->detachFromParent( sourceBoard );
   destination.attach( child, destinationBoard );
   EXPECT_EQ( child->seen.back(), sourceBoard.get() );
   EXPECT_EQ( previous->seen.back(), destinationBoard.get() );
   EXPECT_EQ( source.getState(), Behavior::State::ABORTED );
   EXPECT_EQ( destination.getState(), Behavior::State::ABORTED );
   EXPECT_EQ( sourceBoard->get< bool >( "stopped" ), true );
   EXPECT_EQ( destinationBoard->get< bool >( "stopped" ), true );
   EXPECT_EQ( destination.tick( destinationBoard ), Behavior::State::RUNNING );
   EXPECT_EQ( child->seen.back(), destinationBoard.get() );
   destination.abort( destinationBoard );
}
