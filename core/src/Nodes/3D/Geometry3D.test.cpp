#include "Nodes/3D/Geometry3D.hpp"

#include <gtest/gtest.h>

TEST( Geometry3D, construction )
{
   using namespace crimild::nodes;

   auto g = std::make_shared< Geometry3D >();

   EXPECT_EQ( g->getPrimitive(), nullptr );
   EXPECT_EQ( g->getMaterial(), nullptr );
}

TEST( Geometry3D, coding )
{
   GTEST_SKIP();
}
