/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Codable.hpp"

#include "Decoder.hpp"
#include "Encoder.hpp"
#include "MemoryDecoder.hpp"
#include "MemoryEncoder.hpp"

#include <Crimild_Foundation.hpp>
#include <Crimild_Mathematics.hpp>
#include <gtest/gtest.h>

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

TEST( Codable, codingArray )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableTestObject )

   auto n = crimild::alloc< crimild::CodableTestObject >( "a scene" );
   n->getChildren().add( crimild::alloc< crimild::CodableTestObject >( "child 1" ) );
   n->getChildren().add( crimild::alloc< crimild::CodableTestObject >( "child 2" ) );
   n->getChildren().add( crimild::alloc< crimild::CodableTestObject >( "child 3" ) );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   encoder->encode( n );

   auto bytes = encoder->getBytes();

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   auto n2 = decoder->getObjectAt< crimild::CodableTestObject >( 0 );

   EXPECT_TRUE( n2 != nullptr );

   EXPECT_EQ( n->getChildren().size(), n2->getChildren().size() );
   EXPECT_EQ( n->getChildren()[ 0 ]->getName(), n2->getChildren()[ 0 ]->getName() );
   EXPECT_EQ( n->getChildren()[ 1 ]->getName(), n2->getChildren()[ 1 ]->getName() );
   EXPECT_EQ( n->getChildren()[ 2 ]->getName(), n2->getChildren()[ 2 ]->getName() );
}

TEST( Codable, it_ignores_unknown_types_when_encoding )
{
   GTEST_SKIP() << "Test disabled - needs investigation";

   auto n = crimild::alloc< crimild::CodableTestObject >( "a node" );
   auto child = crimild::alloc< crimild::ChildObject >();
   child->someValue = 10;
   n->getChildren().add( child );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   ASSERT_FALSE( encoder->encode( n ) );
   auto bytes = encoder->getBytes();
   ASSERT_FALSE( bytes.empty() );

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   ASSERT_EQ( 0, decoder->getObjectCount() );
}

TEST( Codable, it_ignores_unknown_types_for_children_when_encoding )
{
   // Register only CodableTestObject type
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableTestObject )

   auto n = crimild::alloc< crimild::CodableTestObject >( "a node" );
   auto child = crimild::alloc< crimild::ChildObject >();
   child->someValue = 10;
   n->getChildren().add( child );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   ASSERT_TRUE( encoder->encode( n ) );
   auto bytes = encoder->getBytes();
   ASSERT_FALSE( bytes.empty() );

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   ASSERT_EQ( 1, decoder->getObjectCount() );
   auto decoded = decoder->getObjectAt< crimild::CodableTestObject >( 0 );
   ASSERT_TRUE( decoded != nullptr );

   EXPECT_EQ( n->getName(), decoded->getName() );
   EXPECT_EQ( 0, decoded->getChildren().size() );
}

TEST( Codable, it_fails_decoding_for_unknown_types )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableTestObject )
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ChildObject )

   auto n = crimild::alloc< crimild::CodableTestObject >( "a node" );
   auto child = crimild::alloc< crimild::ChildObject >();
   child->someValue = 10;
   n->getChildren().add( child );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   ASSERT_TRUE( encoder->encode( n ) );
   auto bytes = encoder->getBytes();

   // Remove component from object factory before decoding
   ObjectFactory::getInstance()->unregisterBuilder( crimild::CodableTestObject::__CLASS_NAME );
   ASSERT_FALSE( ObjectFactory::getInstance()->hasBuilder( crimild::CodableTestObject::__CLASS_NAME ) );

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   ASSERT_EQ( 0, decoder->getObjectCount() );
}

TEST( Codable, it_fails_decoding_for_children_with_unknown_types )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableTestObject )
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ChildObject )

   auto n = crimild::alloc< crimild::CodableTestObject >( "a node" );
   auto child = crimild::alloc< crimild::ChildObject >();
   child->someValue = 10;
   n->getChildren().add( child );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   ASSERT_TRUE( encoder->encode( n ) );
   auto bytes = encoder->getBytes();

   // Remove component from object factory before decoding
   ObjectFactory::getInstance()->unregisterBuilder( crimild::ChildObject::__CLASS_NAME );
   ASSERT_FALSE( ObjectFactory::getInstance()->hasBuilder( crimild::ChildObject::__CLASS_NAME ) );

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   auto decoded = decoder->getObjectAt< crimild::CodableTestObject >( 0 );

   ASSERT_TRUE( decoded != nullptr );

   EXPECT_EQ( n->getName(), decoded->getName() );
   EXPECT_EQ( 0, decoded->getChildren().size() );
}

TEST( Codable, ignores_references_to_unknown_objects )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableTestObject );
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ChildObjectWithReference )

   auto n0 = crimild::alloc< crimild::CodableTestObject >( "a node" );
   auto n1 = crimild::alloc< crimild::CodableTestObject >( "another node" );

   auto child = crimild::alloc< crimild::ChildObjectWithReference >();
   child->other = n1.get();
   n0->getChildren().add( child );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   ASSERT_TRUE( encoder->encode( n0 ) ); // only encode n0
   auto bytes = encoder->getBytes();

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   ASSERT_EQ( 1, decoder->getObjectCount() );

   auto decoded = decoder->getObjectAt< crimild::CodableTestObject >( 0 );
   ASSERT_TRUE( decoded != nullptr );
   EXPECT_EQ( n0->getName(), decoded->getName() );
   ASSERT_EQ( 1, decoded->getChildren().size() );
   auto decodedChild = decoded->getChildren()[ 0 ];
   ASSERT_EQ( nullptr, static_pointer_cast< ChildObjectWithReference >( decodedChild )->other );
}

TEST( Codable, handles_decoding_base_class )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableTestObject );
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ChildObject );

   auto n0 = crimild::alloc< crimild::CodableTestObject >( "a node" );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   ASSERT_TRUE( encoder->encode( n0 ) );
   auto bytes = encoder->getBytes();

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   ASSERT_EQ( 1, decoder->getObjectCount() );

   auto decoded = decoder->getObjectAt< crimild::CodableTestObject >( 0 );
   ASSERT_TRUE( decoded != nullptr );
   EXPECT_EQ( n0->getName(), decoded->getName() );
}

TEST( Codable, fails_to_decode_invalid_type )
{
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableTestObject );
   CRIMILD_REGISTER_OBJECT_BUILDER( crimild::ChildObject );

   auto n0 = crimild::alloc< crimild::CodableTestObject >( "a node" );

   auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
   ASSERT_TRUE( encoder->encode( n0 ) );
   auto bytes = encoder->getBytes();

   auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
   decoder->fromBytes( bytes );
   ASSERT_EQ( 1, decoder->getObjectCount() );

   auto decoded = decoder->getObjectAt< crimild::ChildObject >( 0 );
   ASSERT_TRUE( decoded == nullptr );
}
