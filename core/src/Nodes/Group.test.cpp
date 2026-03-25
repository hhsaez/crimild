#include "Nodes/Group.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST( experimental_Group, attach_child )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group >();
   auto c = std::make_shared< Node >();

   EXPECT_FALSE( p->hasChild( c ) );
   EXPECT_FALSE( c->hasParent() );

   p->attach( c );
   EXPECT_EQ( p->getChildren().size(), 1 );
   EXPECT_EQ( c->getParent(), p );
   EXPECT_TRUE( p->hasChild( c ) );
}

TEST( experimental_Group, detach_child )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group >();
   auto c = std::make_shared< Node >();

   p->attach( c );
   EXPECT_EQ( p->getChildren().size(), 1 );
   EXPECT_TRUE( c->hasParent() );
   EXPECT_EQ( c->getParent(), p );
   EXPECT_TRUE( p->hasChild( c ) );

   p->detach( c );
   EXPECT_EQ( p->getChildren().size(), 0 );
   EXPECT_FALSE( c->hasParent() );
   EXPECT_EQ( c->getParent(), nullptr );
   EXPECT_FALSE( p->hasChild( c ) );
}

TEST( experimental_Group, reattaching_to_parent_does_nothing )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group >();
   auto c = std::make_shared< Node >();

   p->attach( c );
   EXPECT_EQ( p->getChildren().size(), 1 );
   EXPECT_EQ( c->getParent(), p );
   EXPECT_TRUE( p->hasChild( c ) );

   p->attach( c );
   EXPECT_EQ( p->getChildren().size(), 1 );
   EXPECT_EQ( c->getParent(), p );
   EXPECT_TRUE( p->hasChild( c ) );
}

TEST( experimental_Group, switch_parents )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Group >();
   auto p1 = std::make_shared< Group >();
   auto c = std::make_shared< Node >();

   p0->attach( c );
   EXPECT_EQ( c->getParent(), p0 );
   EXPECT_TRUE( p0->hasChild( c ) );

   p1->attach( c );
   EXPECT_EQ( c->getParent(), p1 );
   EXPECT_FALSE( p0->hasChild( c ) );
   EXPECT_TRUE( p1->hasChild( c ) );
}

TEST( experimental_Group, coding )
{
   GTEST_SKIP();
}
