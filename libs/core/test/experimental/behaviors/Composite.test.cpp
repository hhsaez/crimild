#include <crimild/core/experimental/behaviors/composites/Composite.hpp>
#include <gtest/gtest.h>

namespace {

   using crimild::experimental::behaviors::Behavior;
   class TestComposite : public crimild::experimental::behaviors::composites::Composite {
   public:
      TestComposite( void ) = default;
   };

}

TEST( ExperimentalComposite, starts_without_children )
{
   TestComposite composite;
   EXPECT_TRUE( composite.getBehaviors().empty() );
}

TEST( ExperimentalComposite, attaches_children_in_order_without_executing_them )
{
   TestComposite composite;
   auto first = std::make_shared< Behavior >();
   auto second = std::make_shared< Behavior >();
   composite.attach( first );
   composite.attach( second );

   const auto &children = composite.getBehaviors();
   ASSERT_EQ( children.size(), 2 );
   EXPECT_EQ( children[ 0 ], first );
   EXPECT_EQ( children[ 1 ], second );
   EXPECT_EQ( first->getState(), Behavior::State::READY );
   EXPECT_EQ( second->getState(), Behavior::State::READY );
}
