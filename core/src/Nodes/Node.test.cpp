#include "Nodes/Node.hpp"

#include "Nodes/Group.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace crimild::experimental {

   class BaseVisitor {
   public:
      virtual ~BaseVisitor( void ) = default;
   };

   template<
      class VisitableType,
      typename RetType = void >
   class Visitor {
   public:
      using Ret = RetType;
      virtual RetType visit( VisitableType & ) = 0;
   };

   class SomeVisitor
      : public BaseVisitor,
        public Visitor< Node, std::string > {
   public:
      virtual std::string visit( Node &node ) { return node.getName(); }
   };

   template< class VisitableType >
   class Visitable : public VisitableType {
   public:
      virtual void accept( Visitor< VisitableType > &visitor )
      {
         visitor.visit( static_cast< VisitableType & >( *this ) );
      }
   };

   class VisitableNode : public Visitable< Node > {
   public:
   };

}

TEST( experimental_Node, visitors )
{
   using namespace crimild::experimental;
   auto n = std::make_shared< VisitableNode >();
   n->setName( "a node" );

   SomeVisitor v;
   auto s = v.visit( *n );

   EXPECT_EQ( s, "a node" );
}

TEST( experimental_Node, detach_from_parent )
{
   using namespace crimild::experimental;

   auto p = std::make_shared< Group >();
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
