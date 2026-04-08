#include "Nodes/Node.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST( Node, attach_child )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Node >();
   auto c = std::make_shared< Node >();

   EXPECT_FALSE( p->hasChild( c ) );
   EXPECT_FALSE( c->hasParent() );

   p->attach( c );
   EXPECT_EQ( p->getChildren().size(), 1 );
   EXPECT_EQ( c->getParent(), p );
   EXPECT_TRUE( p->hasChild( c ) );
}

TEST( Node, detach_child )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Node >();
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

TEST( Node, reattaching_to_parent_does_nothing )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Node >();
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

TEST( Node, switch_parents )
{
   using namespace crimild::experimental;

   auto p0 = std::make_shared< Node >();
   auto p1 = std::make_shared< Node >();
   auto c = std::make_shared< Node >();

   p0->attach( c );
   EXPECT_EQ( c->getParent(), p0 );
   EXPECT_TRUE( p0->hasChild( c ) );

   p1->attach( c );
   EXPECT_EQ( c->getParent(), p1 );
   EXPECT_FALSE( p0->hasChild( c ) );
   EXPECT_TRUE( p1->hasChild( c ) );
}

TEST( experimental_Node, visitors )
{
   /*
   using namespace crimild::experimental;
   auto n = std::make_shared< VisitableNode >();
   n->setName( "a node" );

   SomeVisitor v;
   auto s = v.visit( *n );

   EXPECT_EQ( s, "a node" );
   */
   GTEST_SKIP();
}

TEST( experimental_Node, detach_from_parent )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Node >();
   auto c = std::make_shared< Node >();

   p->attach( c );
   EXPECT_EQ( p->getChildren().size(), 1 );
   EXPECT_TRUE( c->hasParent() );
   EXPECT_EQ( c->getParent(), p );
   EXPECT_TRUE( p->hasChild( c ) );

   c->detachFromParent();
   EXPECT_EQ( p->getChildren().size(), 0 );
   EXPECT_FALSE( c->hasParent() );
   EXPECT_EQ( c->getParent(), nullptr );
   EXPECT_FALSE( p->hasChild( c ) );
}

TEST( experimental_Node, notify_hierarchy_change_on_children )
{
   /*
   using namespace crimild::experimental;

   class MockNode : public Node {
   public:
      MOCK_METHOD( void, onParentChanged, (), ( override ) );
   };

   using namespace crimild::experimental;

   auto p = std::make_shared< Node >();
   auto c = std::make_shared< MockNode >();

   EXPECT_CALL( *c, onParentChanged ).Times( 1 );
   p->attach( c );
   */
   GTEST_SKIP();
}

TEST( experimental_Node, notify_hierarchy_change_on_indirect_children )
{
   /*
      using namespace crimild::experimental;

      class MockNode : public Node {
      public:
         MOCK_METHOD( void, onParentChanged, (), ( override ) );
      };

      using namespace crimild::experimental;

      auto p0 = std::make_shared< Node >();
      auto p1 = std::make_shared< Node >();
      auto c = std::make_shared< MockNode >();

      EXPECT_CALL( *c, onParentChanged ).Times( 1 );
      p1->attach( c );

      EXPECT_CALL( *c, onParentChanged ).Times( 1 );
      p0->attach( p1 );
   */

   GTEST_SKIP();
}

TEST( experimental_Node, coding )
{
   GTEST_SKIP();
}
