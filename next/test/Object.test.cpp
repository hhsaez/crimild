#include "Object.hpp"

#include "gtest/gtest.h"
 
using namespace crimild::next;
 
TEST( Next_Object, defaultConstruction )
{
    auto obj = crimild::alloc< Object >();

    // EXPECT_EQ( obj->getName(), "" );
}
 
