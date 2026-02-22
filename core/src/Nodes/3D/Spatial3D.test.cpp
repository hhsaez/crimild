#include "Spatial3D.hpp"

#include <gtest/gtest.h>

TEST( Spatial3D, direct_child )
{
   using namespace crimild::nodes;

   auto parent = std::make_shared< Spatial3D >();
   auto child = std::make_shared< Spatial3D >();
   parent->attach( child );

   EXPECT_EQ( child->getParent(), parent );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, indirect_child )
{
   using namespace crimild::nodes;

   auto parent = std::make_shared< Spatial3D >();
   auto node = std::make_shared< Node >();
   parent->attach( node );
   auto child = std::make_shared< Spatial3D >();
   node->attach( child );

   EXPECT_EQ( node->getParent(), parent );
   EXPECT_EQ( child->getParent(), node );
   EXPECT_EQ( child->getParent3D(), parent );
}

TEST( Spatial3D, invalidates_world_when_parent_changes )
{
   using namespace crimild::nodes;

   auto parent = std::make_shared< Spatial3D >();
   auto child = std::make_shared< Spatial3D >();

   EXPECT_TRUE( child->isWorldCurrent() );
   parent->attach( child );
   EXPECT_FALSE( child->isWorldCurrent() );
}

TEST( Spatial3D, invalidates_world_when_hierarchy_changes )
{
   using namespace crimild::nodes;

   auto parent = std::make_shared< Spatial3D >();
   auto node = std::make_shared< Node >();
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
