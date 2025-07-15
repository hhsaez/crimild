#include "MemoryEncoder.hpp"

#include "Tags.hpp"

#include <gtest/gtest.h>

#define CRIMILD_BINARY_HEADER_SIZE 17

using namespace crimild;
using namespace crimild::coding;

TEST( MemoryEncoder, withNoObjects )
{
   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   const auto bytes = encoder->getBytes();

   ASSERT_FALSE( bytes.empty() ) << "Encoding results in no data";

   ASSERT_EQ( bytes.size(), CRIMILD_BINARY_HEADER_SIZE + 1 ); // header + TAG_DATA_END

   ASSERT_EQ( bytes[ 0 ], crimild::coding::Tags::TAG_DATA_START );

   auto versionStr = crimild::Version().getDescription();
   ASSERT_EQ( bytes[ 1 ], crimild::coding::Tags::TAG_DATA_VERSION );
   // str size is 8 bytes, but we only need to read the first one for this simple test
   ASSERT_EQ( bytes[ 2 ], versionStr.length() + 1 ); // don't forget the string termination character
   ASSERT_EQ( bytes[ 10 ], versionStr[ 0 ] );
   ASSERT_EQ( bytes[ 11 ], versionStr[ 1 ] );
   ASSERT_EQ( bytes[ 12 ], versionStr[ 2 ] );
   ASSERT_EQ( bytes[ 13 ], versionStr[ 3 ] );
   ASSERT_EQ( bytes[ 14 ], versionStr[ 4 ] );
   ASSERT_EQ( bytes[ 15 ], versionStr[ 5 ] );
   ASSERT_EQ( bytes[ 16 ], '\0' );

   ASSERT_EQ( bytes[ 17 ], crimild::coding::Tags::TAG_DATA_END );
}

TEST( MemoryEncoder, encodeString )
{
   std::string str = "Hello, World!";

   auto obj = crimild::alloc< EncodedData >( str );
   auto encoder = crimild::alloc< MemoryEncoder >();
   encoder->encode( obj );

   auto bytes = encoder->getBytes();

   auto data = bytes.getData();
   data += CRIMILD_BINARY_HEADER_SIZE;

   ASSERT_EQ( *data, Tags::TAG_OBJECT_BEGIN );
   data += sizeof( Tags::TAG_OBJECT_BEGIN );

   ASSERT_EQ( *reinterpret_cast< Codable::UniqueID * >( data ), obj->getUniqueID() );
   data += sizeof( Codable::UniqueID );

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), std::string( obj->getClassName() ).length() + 1 );
   data += sizeof( uint64_t );

   ASSERT_EQ( std::string( reinterpret_cast< const char * >( data ) ), std::string( obj->getClassName() ) );
   data += std::string( obj->getClassName() ).length() + 1;

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), str.length() + 1 );
   data += sizeof( uint64_t );

   ASSERT_EQ( std::string( reinterpret_cast< const char * >( data ) ), str );
   data += str.length() + 1;

   ASSERT_EQ( *data, Tags::TAG_OBJECT_END );
   data += sizeof( Tags::TAG_OBJECT_END );

   ASSERT_EQ( *data, Tags::TAG_ROOT_OBJECT_BEGIN );
   data += sizeof( Tags::TAG_ROOT_OBJECT_BEGIN );

   ASSERT_EQ( *reinterpret_cast< Codable::UniqueID * >( data ), obj->getUniqueID() );
   data += sizeof( Codable::UniqueID );

   ASSERT_EQ( *data, Tags::TAG_ROOT_OBJECT_END );
   data += sizeof( Tags::TAG_ROOT_OBJECT_END );

   ASSERT_EQ( *data, Tags::TAG_DATA_END );
   data += sizeof( Tags::TAG_DATA_END );

   ASSERT_EQ( data - bytes.getData(), bytes.size() );
}
