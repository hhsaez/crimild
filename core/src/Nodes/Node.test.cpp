#include "Node.hpp"

#include <gtest/gtest.h>

TEST( nodes_Node, test )
{
   using namespace crimild::nodes;

   auto n0 = std::make_shared< Node >();
   EXPECT_FALSE( n0->hasParent() );

   auto n1 = std::make_shared< Node >();
   n0->attach( n1 );
   EXPECT_TRUE( n1->hasParent() );
   EXPECT_EQ( n1->getParent(), n0 );
   // EXPECT_TRUE( n0->attach( n1 ) );

   auto n2 = std::make_shared< Node >();
   n0->attach( n2 );

   EXPECT_TRUE( true );
}

TEST( nodes_Node, notify_hierarchy_change_on_children )
{
   GTEST_SKIP();
}

TEST( nodes_Node, notify_hierarchy_change_on_indirect_children )
{
   GTEST_SKIP();
}

TEST( nodes_Node, coding )
{
   GTEST_SKIP();
}
