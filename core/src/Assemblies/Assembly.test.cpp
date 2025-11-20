#include "Assembly.hpp"

#include "Entity/Entity.hpp"

#include <gtest/gtest.h>

TEST( Assembly, construction )
{
   auto a0 = crimild::alloc< crimild::Assembly >();
   auto a1 = crimild::alloc< crimild::Assembly >( "foo" );
   EXPECT_EQ( a1->getName(), "foo" );
}

TEST( Assembly, entity_management )
{
   auto e = crimild::alloc< crimild::Entity >();
   auto a = crimild::Assembly();
   a.addEntity( e );
   EXPECT_EQ( a.getEntities().size(), 1 );

   a.removeEntity( e );
   EXPECT_TRUE( a.getEntities().empty() );
}

TEST( Assembly, avoid_adding_duplicate_entities )
{
   auto e = crimild::alloc< crimild::Entity >();

   auto a = crimild::alloc< crimild::Assembly >();
   a->addEntity( e );
   EXPECT_EQ( a->getEntities().size(), 1 );
   a->addEntity( e );
   EXPECT_EQ( a->getEntities().size(), 1 );
}

TEST( Assembly, remove_missing_noop )
{
   auto e1 = crimild::alloc< crimild::Entity >();
   auto e2 = crimild::alloc< crimild::Entity >();

   auto a = crimild::alloc< crimild::Assembly >();
   a->addEntity( e1 );
   a->removeEntity( e2 );
   EXPECT_EQ( a->getEntities().size(), 1 );
}

TEST( Assembly, RTTI )
{
   EXPECT_EQ( crimild::Assembly::__CLASS_NAME, "crimild::Assembly" );
}
