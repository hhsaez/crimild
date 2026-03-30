#include "Spatial3D.hpp"

#include "Nodes/3D/Group3D.hpp"
#include "Nodes/Group.hpp"

#include <gtest/gtest.h>

TEST( Spatial3D, no_spatial3D_ancestor )
{
   using namespace crimild::experimental;

   auto spatial = std::make_shared< Spatial3D >();
   EXPECT_FALSE( spatial->hasParent3D() );
   EXPECT_EQ( spatial->getParent3D(), nullptr );
}

TEST( Spatial3D, no_spatial3D_ancestor_in_hierarchy )
{
   using namespace crimild::experimental;

   auto spatial = std::make_shared< Spatial3D >();
   auto p0 = std::make_shared< Group >();
   p0->attach( spatial );
   auto p1 = std::make_shared< Group >();
   p1->attach( p0 );

   EXPECT_FALSE( spatial->hasParent3D() );
   EXPECT_EQ( spatial->getParent3D(), nullptr );
}

TEST( Spatial3D, direct_parent3D )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();
   parent->attach( child );

   EXPECT_EQ( child->getParent(), parent );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, closest_parent3D )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();
   parent->attach( child );
   auto root = std::make_shared< Group3D >();
   root->attach( parent );

   EXPECT_EQ( child->getParent(), parent );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, resets_parent3D_when_detached )
{
   GTEST_FAIL();
}

TEST( Spatial3D, get_parent_indirect_child )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto node = std::make_shared< Group >();
   parent->attach( node );
   auto child = std::make_shared< Spatial3D >();
   node->attach( child );

   EXPECT_EQ( node->getParent(), parent );
   EXPECT_EQ( child->getParent(), node );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, get_closest_parent_indirect_child )
{
   using namespace crimild::experimental;

   auto root = std::make_shared< Group3D >();
   auto parent = std::make_shared< Group3D >();
   root->attach( parent );
   auto node = std::make_shared< Group >();
   parent->attach( node );
   auto child = std::make_shared< Spatial3D >();
   node->attach( child );

   EXPECT_EQ( node->getParent(), parent );
   EXPECT_EQ( child->getParent(), node );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, reparenting )
{
   GTEST_FAIL();
}

TEST( Spatial3D, reparenting_indirect )
{
   GTEST_FAIL();
}

TEST( Spatial3D, resets_indirect_parent3D_when_hierarchy_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, falls_back_to_next_closest_when_removing_closest_ancestor )
{
   using namespace crimild::experimental;

   auto root = std::make_shared< Group3D >();
   auto parent = std::make_shared< Group3D >();
   root->attach( parent );
   auto group = std::make_shared< Group >();
   parent->attach( group );
   auto child = std::make_shared< Spatial3D >();
   group->attach( child );

   EXPECT_EQ( child->getParent(), group );
   EXPECT_EQ( child->getParent3D(), parent );

   parent->detachFromParent();
   root->attach( group );

   EXPECT_EQ( child->getParent(), group );
   EXPECT_EQ( child->getParent3D(), root );
}

TEST( Spatial3D, invalidates_world_when_local_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_local_when_world_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_world_when_direct_parent_local_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_world_when_direct_parent_world_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_world_when_closest_parent_local_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_world_when_closest_parent_world_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_world_when_indirect_ancestor_local_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_world_when_indirect_ancestor_world_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_world_when_attached_to_parent )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   parent->attach( child );
   EXPECT_FALSE( child->isWorldCurrent() );
}

TEST( Spatial3D, does_not_invalidates_world_when_reattached_to_same_parent )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   parent->attach( child );
   EXPECT_FALSE( child->isWorldCurrent() );

   child->getWorld();
   parent->attach( child );
   EXPECT_TRUE( child->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_detached_from_parent )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   parent->attach( child );
   EXPECT_FALSE( child->isWorldCurrent() );

   child->getWorld();
   EXPECT_TRUE( child->isWorldCurrent() );
   child->detachFromParent();
   EXPECT_FALSE( child->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_changing_parents )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   parent->attach( child );
   EXPECT_FALSE( child->isWorldCurrent() );

   child->getWorld();
   EXPECT_TRUE( child->isWorldCurrent() );
   child->detachFromParent();
   EXPECT_FALSE( child->isWorldCurrent() );

   GTEST_FAIL();
}

TEST( Spatial3D, invalidates_world_when_hierarchy_changes )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto node = std::make_shared< Group >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   node->attach( child );
   // Ideally, world should remain true if there is no Spatial3D parent
   EXPECT_FALSE( child->isWorldCurrent() );

   child->getWorld();
   EXPECT_TRUE( child->isWorldCurrent() );

   parent->attach( node );
   EXPECT_FALSE( child->isWorldCurrent() );
}

TEST( Spatial3D, does_not_invalidate_child_on_lazy_world_recompute )
{
   GTEST_FAIL();
}

TEST( Spatial3D, does_not_invalidate_child_on_lazy_local_recompute )
{
   GTEST_FAIL();
}

TEST( Spatial3D, does_not_invalidate_world_state_for_unrelated_sibiling_mutations )
{
   GTEST_FAIL();
}

TEST( Spatial3D, does_not_invalidate_parent_when_child_changes )
{
   GTEST_FAIL();
}

TEST( Spatial3D, computes_world_no_parent )
{
   GTEST_FAIL();
}

TEST( Spatial3D, computes_world_with_parent )
{
   GTEST_FAIL();
}

TEST( Spatial3D, computes_local_no_parent )
{
   GTEST_FAIL();
}

TEST( Spatial3D, computes_local_with_parent )
{
   GTEST_FAIL();
}
