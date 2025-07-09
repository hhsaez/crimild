#include "Codable.hpp"
#include "Decoder.hpp"
#include "Encoder.hpp"

#include <Crimild_Foundation.hpp>
#include <Crimild_Mathematics.hpp>
#include <gtest/gtest.h>

namespace crimild::coding {

   class TextEncoder : public Encoder {
   public:
      virtual ~TextEncoder( void ) noexcept = default;

      virtual crimild::Bool encode( SharedPointer< Codable > const &obj ) override;
      virtual crimild::Bool encode( std::string key, SharedPointer< Codable > const &obj ) override;

      virtual crimild::Bool encode( std::string key, std::string value ) override;

      virtual crimild::Bool encode( std::string key, const Transformation &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::Size value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::UInt8 value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::UInt16 value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::Int16 value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::Int32 value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::UInt32 value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::Real32 value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::Real64 value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const ColorRGB &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const ColorRGBA &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const Point2f &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const Point3f &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const Vector2f &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const Vector3f &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const Vector4f &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const Matrix3f &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const Matrix4f &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, const Quaternion &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, crimild::Bool value ) override { return encodeData( key, value ); }
      // virtual crimild::Bool encode( std::string key, const Format &value ) override { return encodeData( key, value ); }
      // virtual crimild::Bool encode( std::string key, const Extent2D &value ) override { return encodeData( key, value ); }
      // virtual crimild::Bool encode( std::string key, const Extent3D &value ) override { return encodeData( key, value ); }

      virtual bool encode( std::string_view key, std::vector< std::byte > &value ) override
      {
         // TODO: Use std::vector< std::byte > directly instead of converting it to encoded data
         Array< Byte > data;
         data.resize( value.size() );
         memcpy( data.getData(), value.data(), value.size() );
         auto encoded = crimild::alloc< EncodedData >( data );
         return encode( std::string( key ), encoded );
      }

      virtual crimild::Bool encode( std::string key, ByteArray &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, Array< crimild::Real32 > &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, Array< Vector3f > &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, Array< Vector4f > &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, Array< Matrix3f > &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, Array< Matrix4f > &value ) override { return encodeData( key, value ); }
      virtual crimild::Bool encode( std::string key, Array< Quaternion > &value ) override { return encodeData( key, value ); }

      ByteArray getBytes( void ) const;
   };

   class TextDecoder {
   };
}

namespace crimild {

   class CodableTestObject
      : public coding::Codable,
        public Named {
      CRIMILD_IMPLEMENT_RTTI( crimild::CodableTestObject )

   public:
      explicit CodableTestObject( std::string_view name = "" ) noexcept
         : Named( name )
      {
         // no-op
      }

      inline const Array< uint32_t > &getValues( void ) const noexcept { return m_values; }
      inline Array< uint32_t > &getValues( void ) noexcept { return m_values; }

      inline const Array< std::shared_ptr< CodableTestObject > > &getChildren( void ) const noexcept { return m_children; }
      inline Array< std::shared_ptr< CodableTestObject > > &getChildren( void ) noexcept { return m_children; }

      inline void setLocal( const Transformation &t ) noexcept { m_local = t; }
      inline const Transformation &getLocal( void ) const noexcept { return m_local; }

      inline void setWorld( const Transformation &t ) noexcept { m_world = t; }
      inline const Transformation &getWorld( void ) const noexcept { return m_world; }

      inline void setWorldIsCurrent( bool t ) noexcept { m_worldIsCurrent = t; }
      inline bool getWorldIsCurrent( void ) const noexcept { return m_worldIsCurrent; }

   private:
      Array< uint32_t > m_values;
      Array< std::shared_ptr< CodableTestObject > > m_children;
      std::shared_ptr< CodableTestObject > m_anotherChild;
      Transformation m_local;
      Transformation m_world;
      bool m_worldIsCurrent = false;

   public:
      virtual void encode( coding::Encoder &encoder ) override
      {
         Codable::encode( encoder );

         encoder.encode( "name", getName() );
         encoder.encode( "local", m_local );
         encoder.encode( "world", m_world );
         encoder.encode( "worldIsCurrent", m_worldIsCurrent );
         encoder.encode( "values", m_values );
         encoder.encode( "children", m_children );
         encoder.encode( "child", m_anotherChild );
      }

      virtual void decode( coding::Decoder &decoder ) override
      {
         Codable::decode( decoder );

         decoder.decode( "name", getName() );
         decoder.decode( "local", m_local );
         decoder.decode( "world", m_world );
         decoder.decode( "worldIsCurrent", m_worldIsCurrent );
         decoder.decode( "values", m_values );
         decoder.decode( "children", m_children );
         decoder.decode( "child", m_anotherChild );
      }
   };

   class ChildObject : public CodableTestObject {
      CRIMILD_IMPLEMENT_RTTI( crimild::ChildObject )
   public:
      explicit ChildObject( std::string_view name = "" ) noexcept
         : CodableTestObject( name )
      {
      }

      int someValue = 0;

      virtual void encode( coding::Encoder &encoder ) override
      {
         CodableTestObject::encode( encoder );

         encoder.encode( "someValue", someValue );
      }

      virtual void decode( coding::Decoder &decoder ) override
      {
         CodableTestObject::decode( decoder );

         decoder.decode( "someValue", someValue );
      }
   };

   class ChildObjectWithReference : public ChildObject {
      CRIMILD_IMPLEMENT_RTTI( crimild::ChildObjectWithReference )
   public:
      explicit ChildObjectWithReference( std::string_view name = "" ) noexcept
         : ChildObject( name )
      {
      }

      CodableTestObject *other = nullptr;

      virtual void encode( coding::Encoder &encoder ) override
      {
         ChildObject::encode( encoder );

         encoder.encode( "other", other );
      }

      virtual void decode( coding::Decoder &decoder ) override
      {
         ChildObject::decode( decoder );

         auto anotherObject = retain( other );
         decoder.decode( "other", anotherObject );
      }
   };

}

using namespace crimild;

TEST( Codable, simple_encoding )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableTestObject )

   auto n = crimild::alloc< crimild::CodableTestObject >( "a scene" );
   n->getValues() = { 1, 2, 3, 4, 5 };
   n->setLocal( translation( 10, 20, 30 ) );
   n->setWorld( translation( 50, 70, 90 ) );
   n->setWorldIsCurrent( true );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   encoder->encode( n );

   auto bytes = encoder->getBytes();

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   auto n2 = decoder->getObjectAt< crimild::CodableTestObject >( 0 );

   EXPECT_TRUE( n2 != nullptr );

   EXPECT_EQ( n->getName(), n2->getName() );
   EXPECT_EQ( n->getValues(), n2->getValues() );
   EXPECT_EQ( n->getLocal(), n2->getLocal() );
   EXPECT_EQ( n->getWorld(), n2->getWorld() );
   EXPECT_EQ( n->getWorldIsCurrent(), n2->getWorldIsCurrent() );
}
