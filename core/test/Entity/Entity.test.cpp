#include "Entity/Entity.hpp"

#include <gtest/gtest.h>

namespace crimild::test {

   class Foo : public Extension {
      CRIMILD_IMPLEMENT_RTTI( crimild::test::Foo )
   };

}

using namespace crimild;

TEST( Entity, with_extension )
{
   auto entt = crimild::alloc< Entity >();
   auto ext = entt->attach< crimild::test::Foo >();

   ASSERT_TRUE( entt->hasExtension< crimild::test::Foo >() );
   ASSERT_EQ( entt->getExtension< crimild::test::Foo >(), ext );
}

TEST( Entity, coding )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::Entity );
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::test::Foo );

   auto entt = crimild::alloc< Entity >();
   entt->attach< crimild::test::Foo >();

   auto encoder = crimild::alloc< coding::MemoryEncoder >();
   encoder->encode( entt );
   auto bytes = encoder->getBytes();
   auto decoder = crimild::alloc< coding::MemoryDecoder >();
   decoder->fromBytes( bytes );

   auto decoded = decoder->getObjectAt< Entity >( 0 );
   ASSERT_NE( decoded, nullptr );
   ASSERT_TRUE( decoded->hasExtension< crimild::test::Foo >() );
}
