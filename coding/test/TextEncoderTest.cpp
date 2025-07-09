#include "TextEncoder.hpp"

#include <gtest/gtest.h>

namespace crimild::coding::test {

   template< typename ValueType >
   class ValueWrapper : public Codable {
      CRIMILD_IMPLEMENT_RTTI( crimild::coding::test::ValueWrapper )

   public:
      ValueType value;

      virtual void encode( coding::Encoder &encoder ) override
      {
         Codable::encode( encoder );

         encoder.encode( "value", value );
      }
   };

   class StringWrapper : public ValueWrapper< std::string > {
      CRIMILD_IMPLEMENT_RTTI( crimild::coding::test::StringWrapper )
   };

}

TEST( TextEncoder, encodeString )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::coding::test::StringWrapper )

   auto obj = crimild::alloc< crimild::coding::test::StringWrapper >();
   obj->value = "This is a test string";

   auto encoder = crimild::alloc< crimild::coding::TextEncoder >();
   encoder->encode( obj );

   auto str = encoder->getString();
   std::string expected = R"(
      CRIMILD v5.0.0

      [id=12345, className="crimild::coding::test::StringWrapper"]
      value = "This is a test string"
   )";

   EXPECT_EQ( expected, str );
}
