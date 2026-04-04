#include "Spatial3D.hpp"

#include "Nodes/3D/Group3D.hpp"
#include "Nodes/Group.hpp"
#include "translation.hpp"

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
   auto p1 = std::make_shared< Group >();

   p1->attach( p0 );
   p0->attach( spatial );

   EXPECT_FALSE( spatial->hasParent3D() );
   EXPECT_EQ( spatial->getParent3D(), nullptr );
}

TEST( Spatial3D, direct_parent3D )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   parent->attach( child );

   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), parent );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, closest_parent3D )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();
   auto root = std::make_shared< Group3D >();

   root->attach( parent );
   parent->attach( child );

   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), parent );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, resets_parent3D_when_detached )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   parent->attach( child );
   child->detachFromParent();

   EXPECT_FALSE( child->hasParent() );
   EXPECT_EQ( child->getParent(), nullptr );
   EXPECT_FALSE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), nullptr );
}

TEST( Spatial3D, get_closest_parent_indirect )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto node = std::make_shared< Group >(); // not Spatial3D
   auto child = std::make_shared< Spatial3D >();

   parent->attach( node );
   node->attach( child );

   EXPECT_EQ( node->getParent(), parent );
   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), node );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, get_closest_parent_in_hierarchy_indirect )
{
   using namespace crimild::experimental;

   auto root = std::make_shared< Group3D >();
   auto parent = std::make_shared< Group3D >();
   auto node = std::make_shared< Group >(); // Not Spatial3D
   auto child = std::make_shared< Spatial3D >();

   root->attach( parent );
   parent->attach( node );
   node->attach( child );

   EXPECT_EQ( node->getParent(), parent );
   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), node );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, reparenting )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group3D >();
   auto p1 = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   p0->attach( child );

   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), p0 );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), p0 );

   p1->attach( child );

   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), p1 );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_NE( child->getParent3D(), p0 );
   EXPECT_EQ( child->getParent3D(), p1 );
}

TEST( Spatial3D, reparenting_indirect )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group3D >();
   auto p1 = std::make_shared< Group3D >();
   auto g0 = std::make_shared< Group >(); // non spatial
   auto g1 = std::make_shared< Group >(); // non spatial
   auto child = std::make_shared< Spatial3D >();

   p0->attach( g0 );
   g0->attach( child );

   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), g0 );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), p0 );

   p1->attach( g1 );
   g1->attach( child );

   EXPECT_FALSE( g0->hasChild( child ) );
   EXPECT_TRUE( g1->hasChild( child ) );
   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), g1 );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), p1 );
}

TEST( Spatial3D, reparenting_non_spatial )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group3D >();
   auto p1 = std::make_shared< Group >(); // Not Spatial3D
   auto child = std::make_shared< Spatial3D >();

   p0->attach( child );

   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), p0 );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), p0 );

   p1->attach( child );

   EXPECT_FALSE( p0->hasChild( child ) );
   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), p1 );
   EXPECT_FALSE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), nullptr );
}

TEST( Spatial3D, get_closest_indirect_parent3D_when_hierarchy_changes )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group3D >();
   auto g0 = std::make_shared< Group >();
   auto child = std::make_shared< Spatial3D >();

   g0->attach( child );

   EXPECT_TRUE( g0->hasChild( child ) );
   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), g0 );
   EXPECT_FALSE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), nullptr );

   p0->attach( g0 );

   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), g0 );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), p0 );
}
n TEST( Spatial3D, resets_parent3D_when_hierarchy_changes )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group3D >();
   auto g0 = std::make_shared< Group >();
   auto child = std::make_shared< Spatial3D >();

   p0->attach( g0 );
   g0->attach( child );

   EXPECT_TRUE( g0->hasChild( child ) );
   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), g0 );
   EXPECT_TRUE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), p0 );

   g0->detachFromParent();

   EXPECT_TRUE( child->hasParent() );
   EXPECT_EQ( child->getParent(), g0 );
   EXPECT_FALSE( child->hasParent3D() );
   EXPECT_EQ( child->getParent3D(), nullptr );
}

TEST( Spatial3D, falls_back_to_next_closest_when_removing_closest_ancestor )
{
   using namespace crimild::experimental;

   auto root = std::make_shared< Group3D >();
   auto parent = std::make_shared< Group3D >();
   auto group = std::make_shared< Group >();
   auto child = std::make_shared< Spatial3D >();

   root->attach( parent );
   parent->attach( group );
   group->attach( child );

   EXPECT_EQ( child->getParent(), group );
   EXPECT_EQ( child->getParent3D(), parent );

   parent->detachFromParent();

   EXPECT_EQ( child->getParent(), group );
   EXPECT_EQ( child->getParent3D(), parent );

   root->attach( group );

   EXPECT_EQ( child->getParent(), group );
   EXPECT_EQ( child->getParent3D(), root );
}

TEST( Spatial3D, falls_back_to_next_closest_when_hierarchy_changes )
{
   using namespace crimild::experimental;

   auto root = std::make_shared< Group3D >();
   auto parent = std::make_shared< Group3D >();
   auto group = std::make_shared< Group >();
   auto child = std::make_shared< Spatial3D >();

   root->attach( parent );
   parent->attach( group );
   group->attach( child );

   EXPECT_EQ( child->getParent(), group );
   EXPECT_EQ( child->getParent3D(), parent );

   root->attach( group );

   EXPECT_EQ( child->getParent(), group );
   EXPECT_EQ( child->getParent3D(), root );
}

TEST( Spatial3D, default_world_state )
{
   using namespace crimild::experimental;

   auto s = std::make_shared< Spatial3D >();

   EXPECT_TRUE( s->isLocalCurrent() );
   EXPECT_EQ( s->getLocal(), crimild::Transformation::Constants::IDENTITY );
   EXPECT_TRUE( s->isWorldCurrent() );
   EXPECT_EQ( s->getWorld(), crimild::Transformation::Constants::IDENTITY );
}

TEST( Spatial3D, invalidates_world_when_local_changes )
{
   using namespace crimild::experimental;

   auto s = std::make_shared< Spatial3D >();

   EXPECT_TRUE( s->isLocalCurrent() );
   EXPECT_TRUE( s->isWorldCurrent() );

   s->setLocal( crimild::Transformation {} );

   EXPECT_TRUE( s->isLocalCurrent() );
   EXPECT_FALSE( s->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_local_when_world_changes )
{
   using namespace crimild::experimental;

   auto s = std::make_shared< Spatial3D >();

   EXPECT_TRUE( s->isLocalCurrent() );
   EXPECT_TRUE( s->isWorldCurrent() );

   s->setWorld( crimild::Transformation {} );

   EXPECT_FALSE( s->isLocalCurrent() );
   EXPECT_TRUE( s->isWorldCurrent() );
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

TEST( Spatial3D, invalidates_world_when_direct_parent_local_changes )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto c = std::make_shared< Spatial3D >();

   p->attach( c );

   EXPECT_FALSE( c->isWorldCurrent() );

   c->getWorld();

   EXPECT_TRUE( c->isWorldCurrent() );

   p->setLocal( crimild::Transformation {} );

   EXPECT_FALSE( c->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_direct_parent_world_changes )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto c = std::make_shared< Spatial3D >();

   p->attach( c );

   EXPECT_FALSE( c->isWorldCurrent() );

   c->getWorld();

   EXPECT_TRUE( c->isWorldCurrent() );

   p->setWorld( crimild::Transformation {} );

   EXPECT_FALSE( c->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_closest_parent_local_changes )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto g = std::make_shared< Group >();
   auto c = std::make_shared< Spatial3D >();

   p->attach( g );
   g->attach( c );

   c->getWorld(); // reset
   EXPECT_TRUE( c->isWorldCurrent() );

   p->setLocal( crimild::Transformation {} );
   EXPECT_FALSE( c->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_closest_parent_world_changes )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto g = std::make_shared< Group >();
   auto c = std::make_shared< Spatial3D >();

   p->attach( g );
   g->attach( c );

   c->getWorld(); // reset
   EXPECT_TRUE( c->isWorldCurrent() );

   p->setWorld( crimild::Transformation {} );
   EXPECT_FALSE( c->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_indirect_ancestor_local_changes )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group3D >();
   auto p1 = std::make_shared< Group3D >();
   auto c = std::make_shared< Spatial3D >();

   p0->attach( p1 );
   p1->attach( c );

   EXPECT_EQ( c->getParent3D(), p1 );

   c->getWorld(); // reset
   EXPECT_TRUE( c->isWorldCurrent() );

   p0->setLocal( crimild::Transformation {} );
   EXPECT_FALSE( p1->isWorldCurrent() );
   EXPECT_FALSE( c->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_indirect_ancestor_world_changes )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group3D >();
   auto p1 = std::make_shared< Group3D >();
   auto c = std::make_shared< Spatial3D >();

   p0->attach( p1 );
   p1->attach( c );

   EXPECT_EQ( c->getParent3D(), p1 );

   c->getWorld(); // reset
   EXPECT_TRUE( c->isWorldCurrent() );

   p0->setWorld( crimild::Transformation {} );
   EXPECT_FALSE( p1->isWorldCurrent() );
   EXPECT_FALSE( c->isWorldCurrent() );
}

TEST( Spatial3D, does_not_invalidates_world_when_reattached_to_same_parent )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   parent->attach( child );
   EXPECT_FALSE( child->isWorldCurrent() );

   child->getWorld(); // reset

   parent->attach( child ); // no-op
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

   child->getWorld(); // reset
   EXPECT_TRUE( child->isWorldCurrent() );

   child->detachFromParent();
   EXPECT_FALSE( child->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_switching_parents )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group3D >();
   auto p1 = std::make_shared< Group3D >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   p0->attach( child );
   EXPECT_FALSE( child->isWorldCurrent() );

   child->getWorld(); // reset
   EXPECT_TRUE( child->isWorldCurrent() );

   p1->attach( child );
   EXPECT_EQ( child->getParent3D(), p1 );
   EXPECT_FALSE( child->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_hierarchy_changes )
{
   using namespace crimild::experimental;

   auto parent = std::make_shared< Group3D >();
   auto node = std::make_shared< Group >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   node->attach( child );
   EXPECT_FALSE( child->isWorldCurrent() );

   child->getWorld(); // reset
   EXPECT_TRUE( child->isWorldCurrent() );

   parent->attach( node );
   EXPECT_FALSE( child->isWorldCurrent() );
}

TEST( Spatial3D, does_not_invalidate_world_on_lazy_world_recompute )
{
   using namespace crimild::experimental;

   auto s = std::make_shared< Spatial3D >();

   EXPECT_TRUE( s->isWorldCurrent() );

   s->setLocal( crimild::Transformation {} );

   EXPECT_FALSE( s->isWorldCurrent() );

   s->getWorld();

   EXPECT_TRUE( s->isWorldCurrent() );
}

TEST( Spatial3D, does_not_invalidate_world_on_lazy_local_recompute )
{
   using namespace crimild::experimental;

   auto s = std::make_shared< Spatial3D >();

   EXPECT_TRUE( s->isWorldCurrent() );

   s->setWorld( crimild::Transformation {} );

   EXPECT_FALSE( s->isLocalCurrent() );
   EXPECT_TRUE( s->isWorldCurrent() );

   s->getLocal();

   EXPECT_TRUE( s->isLocalCurrent() );
   EXPECT_TRUE( s->isWorldCurrent() );
}

TEST( Spatial3D, does_not_invalidate_world_state_for_unrelated_sibiling_mutations )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto c0 = std::make_shared< Spatial3D >();
   auto c1 = std::make_shared< Spatial3D >();

   p->attach( c0 );
   p->attach( c1 );

   c0->getWorld(); // reset
   c1->getWorld(); // reset

   EXPECT_TRUE( c0->isWorldCurrent() );
   EXPECT_TRUE( c1->isWorldCurrent() );

   c1->setLocal( crimild::Transformation {} );

   EXPECT_TRUE( c0->isWorldCurrent() );
   EXPECT_FALSE( c1->isWorldCurrent() );
}

TEST( Spatial3D, does_not_invalidate_parent_when_child_changes )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto c = std::make_shared< Spatial3D >();

   p->attach( c );

   c->getWorld(); // reset

   EXPECT_TRUE( p->isWorldCurrent() );
   EXPECT_TRUE( c->isWorldCurrent() );

   c->setLocal( crimild::Transformation {} );

   EXPECT_TRUE( p->isWorldCurrent() );
   EXPECT_FALSE( c->isWorldCurrent() );
}

TEST( Spatial3D, computes_world_from_local_no_parent )
{
   using namespace crimild::experimental;

   auto s = std::make_shared< Spatial3D >();

   s->setLocal( crimild::translation( 1, 2, 3 ) );

   EXPECT_EQ( s->getLocal(), crimild::translation( 1, 2, 3 ) );

   EXPECT_FALSE( s->isWorldCurrent() );
   EXPECT_EQ( s->getWorld(), s->getLocal() );
   EXPECT_TRUE( s->isWorldCurrent() );
}

TEST( Spatial3D, computes_local_from_world_no_parent )
{
   using namespace crimild::experimental;

   auto s = std::make_shared< Spatial3D >();

   s->setWorld( crimild::translation( 1, 2, 3 ) );

   EXPECT_EQ( s->getWorld(), crimild::translation( 1, 2, 3 ) );

   EXPECT_FALSE( s->isLocalCurrent() );
   EXPECT_EQ( s->getLocal(), s->getWorld() );
   EXPECT_TRUE( s->isLocalCurrent() );
}

TEST( Spatial3D, computes_world_from_local_with_parent )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto c = std::make_shared< Spatial3D >();

   p->attach( c );

   p->setLocal( crimild::translation( 1, 2, 3 ) );
   c->setLocal( crimild::translation( 4, 5, 6 ) );

   EXPECT_FALSE( p->isWorldCurrent() );
   EXPECT_EQ( p->getWorld(), p->getLocal() );
   EXPECT_TRUE( p->isWorldCurrent() );

   EXPECT_FALSE( c->isWorldCurrent() );
   EXPECT_EQ( c->getWorld(), p->getWorld()( c->getLocal() ) );
   EXPECT_TRUE( c->isWorldCurrent() );
}

TEST( Spatial3D, computes_world_from_world_with_parent )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto c = std::make_shared< Spatial3D >();

   p->attach( c );

   p->setLocal( crimild::translation( 1, 2, 3 ) );

   EXPECT_FALSE( p->isWorldCurrent() );
   EXPECT_EQ( p->getWorld(), p->getLocal() );
   EXPECT_TRUE( p->isWorldCurrent() );

   // handles spacial case when changing world and there's a parent
   // world should be recomputed by considering parent's own world transform
   c->setWorld( crimild::translation( 4, 5, 6 ) );

   EXPECT_TRUE( c->isWorldCurrent() );
   EXPECT_EQ( c->getWorld(), p->getWorld()( c->getLocal() ) );
   EXPECT_EQ( c->getWorld(), crimild::translation( 4, 5, 6 ) );
   EXPECT_TRUE( c->isWorldCurrent() );
}

TEST( Spatial3D, computes_local_from_world_with_parent )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group3D >();
   auto c = std::make_shared< Spatial3D >();

   p->attach( c );

   p->setLocal( crimild::translation( 1, 2, 3 ) );
   c->setWorld( crimild::translation( 4, 5, 6 ) );

   EXPECT_FALSE( p->isWorldCurrent() );
   EXPECT_EQ( p->getWorld(), p->getLocal() );
   EXPECT_TRUE( p->isWorldCurrent() );

   EXPECT_FALSE( c->isLocalCurrent() );
   EXPECT_EQ( c->getWorld(), p->getWorld()( c->getLocal() ) );
   EXPECT_EQ( c->getLocal(), crimild::inverse( p->getWorld() )( c->getWorld() ) );
   EXPECT_TRUE( c->isLocalCurrent() );
   EXPECT_TRUE( c->isWorldCurrent() );
}
