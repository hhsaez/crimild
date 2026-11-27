#include <crimild/core/experimental/nodes/events/OnTick.hpp>
#include <crimild/core/experimental/systems/TickSystem.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {

   using crimild::experimental::Blackboard;
   using crimild::experimental::Node;
   using crimild::experimental::OnTick;
   using crimild::experimental::behaviors::Behavior;
   using crimild::experimental::systems::Context;
   using crimild::experimental::systems::System;
   using crimild::experimental::systems::TickSystem;
   namespace systemKeys = crimild::experimental::systems::keys;

   class Action : public Behavior {
   public:
      int calls = 0;
      State result = State::RUNNING;
      std::vector< double > times;

   protected:
      State update( Blackboard &board ) override
      {
         ++calls;
         const auto clock = board.get( crimild::experimental::keys::Clock );
         EXPECT_TRUE( clock );
         if ( clock ) {
            times.push_back( clock->getCurrentTime() );
         }
         return result;
      }
   };

}

TEST( experimental_TickSystem, missing_or_null_scene_is_a_system_failure )
{
   Context context;
   TickSystem missing;
   EXPECT_EQ( missing.update( context ), System::State::FAILURE );
   context.set( systemKeys::Scene, std::shared_ptr< Node > {} );
   TickSystem invalid;
   EXPECT_EQ( invalid.update( context ), System::State::FAILURE );
}

TEST( experimental_TickSystem, traverses_scene_events_with_one_clock_per_frame )
{
   Context context;
   const auto scene = std::make_shared< Node >();
   context.set( systemKeys::Scene, scene );
   const auto first = scene->attach< OnTick >();
   const auto second = scene->attach< Node >()->attach< OnTick >();
   const auto left = std::make_shared< Action >();
   const auto right = std::make_shared< Action >();
   first->setBehavior( left );
   second->setBehavior( right );
   TickSystem system;
   EXPECT_EQ( system.update( context ), System::State::SUCCESS );
   EXPECT_EQ( system.update( context ), System::State::SUCCESS );
   EXPECT_EQ( left->calls, 2 );
   EXPECT_EQ( right->calls, 2 );
   EXPECT_EQ( left->times, right->times );
   EXPECT_EQ( left->getState(), Behavior::State::RUNNING );
}

TEST( experimental_TickSystem, skips_disabled_events_and_disabled_subtrees )
{
   Context context;
   const auto scene = std::make_shared< Node >();
   context.set( systemKeys::Scene, scene );
   const auto disabledEvent = scene->attach< OnTick >();
   disabledEvent->setEnabled( false );
   const auto disabledParent = scene->attach< Node >();
   disabledParent->setEnabled( false );
   const auto hiddenEvent = disabledParent->attach< OnTick >();
   const auto visibleEvent = scene->attach< OnTick >();
   const auto disabledAction = std::make_shared< Action >();
   const auto hiddenAction = std::make_shared< Action >();
   const auto visibleAction = std::make_shared< Action >();
   disabledEvent->setBehavior( disabledAction );
   hiddenEvent->setBehavior( hiddenAction );
   visibleEvent->setBehavior( visibleAction );
   TickSystem system;
   EXPECT_EQ( system.update( context ), System::State::SUCCESS );
   EXPECT_EQ( disabledAction->calls, 0 );
   EXPECT_EQ( hiddenAction->calls, 0 );
   EXPECT_EQ( visibleAction->calls, 1 );
}

TEST( experimental_TickSystem, behavior_failure_does_not_fail_the_system_or_skip_other_events )
{
   Context context;
   const auto scene = std::make_shared< Node >();
   context.set( systemKeys::Scene, scene );
   const auto failed = std::make_shared< Action >();
   failed->result = Behavior::State::FAILURE;
   const auto running = std::make_shared< Action >();
   scene->attach< OnTick >()->setBehavior( failed );
   scene->attach< OnTick >()->setBehavior( running );
   TickSystem system;
   EXPECT_EQ( system.update( context ), System::State::SUCCESS );
   EXPECT_EQ( failed->calls, 1 );
   EXPECT_EQ( running->calls, 1 );
}

TEST( experimental_TickSystem, propagates_child_system_results_after_ticking_the_scene )
{
   class ChildSystem : public System {
   public:
      int calls = 0;

   protected:
      State onUpdate( const Context & ) override
      {
         ++calls;
         return State::RUNNING;
      }
   };
   Context context;
   context.set( systemKeys::Scene, std::make_shared< Node >() );
   TickSystem system;
   const auto child = std::make_shared< ChildSystem >();
   system.attach( child );
   EXPECT_EQ( system.update( context ), System::State::RUNNING );
   EXPECT_EQ( child->calls, 1 );
}
