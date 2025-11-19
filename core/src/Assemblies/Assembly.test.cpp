#include "Assembly.hpp"

#include <gtest/gtest.h>

TEST( Assembly, construction )
{
   auto a0 = crimild::alloc< crimild::Assembly >();
   auto a1 = crimild::alloc< crimild::Assembly >( "foo" );
   EXPECT_EQ( a1->getName(), "foo" );
}
