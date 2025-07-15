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

TEST( MemoryEncoder, encodeSize )
{
   crimild::Size value = 42;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::Size ) );
   data += sizeof( uint64_t );

   ASSERT_EQ( *reinterpret_cast< crimild::Size * >( data ), value );
   data += sizeof( crimild::Size );

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

TEST( MemoryEncoder, encodeInt32 )
{
   crimild::Int32 value = -123;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::Int32 ) );
   data += sizeof( uint64_t );

   ASSERT_EQ( *reinterpret_cast< crimild::Int32 * >( data ), value );
   data += sizeof( crimild::Int32 );

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

TEST( MemoryEncoder, encodeReal32 )
{
   crimild::Real32 value = 3.14159f;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::Real32 ) );
   data += sizeof( uint64_t );

   ASSERT_FLOAT_EQ( *reinterpret_cast< crimild::Real32 * >( data ), value );
   data += sizeof( crimild::Real32 );

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

TEST( MemoryEncoder, encodeBool )
{
   crimild::Bool value = true;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::Bool ) );
   data += sizeof( uint64_t );

   ASSERT_EQ( *reinterpret_cast< crimild::Bool * >( data ), value );
   data += sizeof( crimild::Bool );

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

TEST( MemoryEncoder, encodeUInt8 )
{
   crimild::UInt8 value = 255;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::UInt8 ) );
   data += sizeof( uint64_t );

   ASSERT_EQ( *reinterpret_cast< crimild::UInt8 * >( data ), value );
   data += sizeof( crimild::UInt8 );

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

TEST( MemoryEncoder, encodeUInt16 )
{
   crimild::UInt16 value = 65535;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::UInt16 ) );
   data += sizeof( uint64_t );

   ASSERT_EQ( *reinterpret_cast< crimild::UInt16 * >( data ), value );
   data += sizeof( crimild::UInt16 );

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

TEST( MemoryEncoder, encodeInt16 )
{
   crimild::Int16 value = -32768;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::Int16 ) );
   data += sizeof( uint64_t );

   ASSERT_EQ( *reinterpret_cast< crimild::Int16 * >( data ), value );
   data += sizeof( crimild::Int16 );

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

TEST( MemoryEncoder, encodeUInt32 )
{
   crimild::UInt32 value = 4294967295U;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::UInt32 ) );
   data += sizeof( uint64_t );

   ASSERT_EQ( *reinterpret_cast< crimild::UInt32 * >( data ), value );
   data += sizeof( crimild::UInt32 );

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

TEST( MemoryEncoder, encodeReal64 )
{
   crimild::Real64 value = 3.141592653589793;

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( crimild::Real64 ) );
   data += sizeof( uint64_t );

   ASSERT_DOUBLE_EQ( *reinterpret_cast< crimild::Real64 * >( data ), value );
   data += sizeof( crimild::Real64 );

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

TEST( MemoryEncoder, encodeVector3f )
{
   Vector3f value( 1.0f, 2.5f, -3.7f );

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( Vector3f ) );
   data += sizeof( uint64_t );

   auto decodedValue = *reinterpret_cast< Vector3f * >( data );
   ASSERT_FLOAT_EQ( decodedValue[ 0 ], value[ 0 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 1 ], value[ 1 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 2 ], value[ 2 ] );
   data += sizeof( Vector3f );

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

TEST( MemoryEncoder, encodeVector2f )
{
   Vector2f value( 4.2f, -1.8f );

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( Vector2f ) );
   data += sizeof( uint64_t );

   auto decodedValue = *reinterpret_cast< Vector2f * >( data );
   ASSERT_FLOAT_EQ( decodedValue[ 0 ], value[ 0 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 1 ], value[ 1 ] );
   data += sizeof( Vector2f );

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

TEST( MemoryEncoder, encodeVector4f )
{
   Vector4f value( 1.0f, 2.0f, 3.0f, 4.0f );

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( Vector4f ) );
   data += sizeof( uint64_t );

   auto decodedValue = *reinterpret_cast< Vector4f * >( data );
   ASSERT_FLOAT_EQ( decodedValue[ 0 ], value[ 0 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 1 ], value[ 1 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 2 ], value[ 2 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 3 ], value[ 3 ] );
   data += sizeof( Vector4f );

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

TEST( MemoryEncoder, encodeQuaternion )
{
   Quaternion value( 1.0f, 0.0f, 0.0f, 0.0f );

   auto obj = crimild::alloc< EncodedData >( value );
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

   ASSERT_EQ( *reinterpret_cast< uint64_t * >( data ), sizeof( Quaternion ) );
   data += sizeof( uint64_t );

   auto decodedValue = *reinterpret_cast< Quaternion * >( data );
   ASSERT_FLOAT_EQ( decodedValue[ 0 ], value[ 0 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 1 ], value[ 1 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 2 ], value[ 2 ] );
   ASSERT_FLOAT_EQ( decodedValue[ 3 ], value[ 3 ] );
   data += sizeof( Quaternion );

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
