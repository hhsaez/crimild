#include "TextEncoder.hpp"

#include <Crimild_Foundation.hpp>
#include <format>
#include <gtest/gtest.h>
#include <sstream>

namespace crimild {

   static std::string trimLines( const std::string &text )
   {
      std::istringstream iss( text );
      std::ostringstream result;
      std::string line;
      bool first = true;

      while ( std::getline( iss, line ) ) {
         // Trim leading whitespace
         auto start = line.find_first_not_of( " \t\r" );

         // Trim trailing whitespace
         auto end = line.find_last_not_of( " \t\r" );

         auto str = start != std::string::npos ? line.substr( start, end - start + 1 ) : "";
         if ( first && str.empty() ) {
            // Skip empty lines only at the beginning
            continue;
         }

         if ( !first ) {
            result << '\n';
         }
         first = false;

         result << str;
      }

      return result.str();
   }

   template< typename... Args >
   static std::string format( const char *fmt, Args &&...args )
   {
      std::string formatted;
      // Create format args with proper forwarding
      auto format_args = std::make_format_args( args... );
      // Use string_view for format string with vformat
      formatted = std::vformat( std::string_view( fmt ), format_args );
      return trimLines( formatted );
   }
}

namespace crimild::coding::test {

   class TestObject
      : public Codable,
        public Named {
   public:
      CRIMILD_IMPLEMENT_RTTI( crimild::coding::test::TestObject )

      explicit TestObject( std::string name = "Test Object" ) noexcept : Named( name ) { }
      virtual ~TestObject( void ) noexcept = default;

      float floatValue;
      int32_t intValue;
      std::shared_ptr< TestObject > child;

      virtual void encode( Encoder &encoder ) override
      {
         Codable::encode( encoder );

         // encoder.encode( "name", getName() );
         // encoder.encode( "floatValue", floatValue );
         // encoder.encode( "intValue", intValue );
      }

      virtual void decode( Decoder &decoder ) override
      {
         Codable::decode( decoder );
      }
   };

}

TEST( TextEncoder, basicEncoding )
{
   GTEST_SKIP() << "Test disabled - needs further work";

   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::coding::test::TestObject )

   auto obj = std::make_shared< crimild::coding::test::TestObject >();
   ASSERT_NE( obj, nullptr );

   obj->setName( "Test Object" );
   ASSERT_EQ( obj->getName(), "Test Object" );

   obj->floatValue = 123.45f;
   obj->intValue = 42;

   auto encoder = crimild::alloc< crimild::coding::TextEncoder >();
   // encoder->encode( obj );

   auto str = encoder->getString();
   std::string expected = crimild::format(
      R"(
         [CRIMILD {}]

         [id={}, className="crimild::coding::test::TestValue"]
         name = "Test Object"
         floatValue = 123.45
         intValue = 42
      )",
      crimild::Version().getDescription(),
      obj->getUniqueID()
   );

   EXPECT_EQ( expected, str );
}
