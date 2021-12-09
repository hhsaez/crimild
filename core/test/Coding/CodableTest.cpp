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

#include "Coding/Codable.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Foundation/ObjectFactory.hpp"
#include "Mathematics/Transformation_equality.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Node.hpp"

#include "gtest/gtest.h"

namespace crimild {

    class CodableNode : public Node {
        CRIMILD_IMPLEMENT_RTTI( crimild::CodableNode )

    public:
        explicit CodableNode( std::string name = "" )
            : Node( name ) { }
        virtual ~CodableNode( void ) { }

        Array< int > &getValues( void ) { return _values; }
        Array< SharedPointer< CodableNode > > &getChildren( void ) { return _children; }

    private:
        Array< int > _values;
        Array< SharedPointer< CodableNode > > _children;
        SharedPointer< Group > _aGroup;

    public:
        virtual void encode( coding::Encoder &encoder ) override
        {
            Node::encode( encoder );

            encoder.encode( "values", _values );
            encoder.encode( "children", _children );
            encoder.encode( "group", _aGroup );
        }

        virtual void decode( coding::Decoder &decoder ) override
        {
            Node::decode( decoder );

            decoder.decode( "values", _values );
            decoder.decode( "children", _children );
            decoder.decode( "group", _aGroup );
        }
    };

}

using namespace crimild;

TEST( CodableTest, codingEncoding )
{
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableNode )

    auto n = crimild::alloc< crimild::CodableNode >( "a scene" );
    n->getValues() = { 1, 2, 3, 4, 5 };
    n->setLocal( translation( 10, 20, 30 ) );
    n->setWorld( translation( 50, 70, 90 ) );
    n->setWorldIsCurrent( true );

    auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
    encoder->encode( n );

    auto bytes = encoder->getBytes();

    auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
    decoder->fromBytes( bytes );
    auto n2 = decoder->getObjectAt< crimild::CodableNode >( 0 );

    EXPECT_TRUE( n2 != nullptr );

    EXPECT_EQ( n->getName(), n2->getName() );
    EXPECT_EQ( n->getValues(), n2->getValues() );
    EXPECT_EQ( n->getLocal(), n2->getLocal() );
    EXPECT_EQ( n->getWorld(), n2->getWorld() );
    EXPECT_EQ( n->worldIsCurrent(), n2->worldIsCurrent() );
}

TEST( CodableTest, codingArray )
{
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableNode )

    auto n = crimild::alloc< crimild::CodableNode >( "a scene" );
    n->getChildren().add( crimild::alloc< crimild::CodableNode >( "child 1" ) );
    n->getChildren().add( crimild::alloc< crimild::CodableNode >( "child 2" ) );
    n->getChildren().add( crimild::alloc< crimild::CodableNode >( "child 3" ) );

    auto encoder = crimild::alloc< crimild::coding::MemoryEncoder >();
    encoder->encode( n );

    auto bytes = encoder->getBytes();

    auto decoder = crimild::alloc< crimild::coding::MemoryDecoder >();
    decoder->fromBytes( bytes );
    auto n2 = decoder->getObjectAt< crimild::CodableNode >( 0 );

    EXPECT_TRUE( n2 != nullptr );

    EXPECT_EQ( n->getChildren().size(), n2->getChildren().size() );
    EXPECT_EQ( n->getChildren()[ 0 ]->getName(), n2->getChildren()[ 0 ]->getName() );
    EXPECT_EQ( n->getChildren()[ 1 ]->getName(), n2->getChildren()[ 1 ]->getName() );
    EXPECT_EQ( n->getChildren()[ 2 ]->getName(), n2->getChildren()[ 2 ]->getName() );
}

namespace crimild {

    namespace coding {

        class MutableMemoryCoder : public coding::Encoder {
        public:
            virtual crimild::Bool encode( SharedPointer< coding::Codable > const &codable ) override
            {
                // TODO

                codable->encode( *this );

                return true;
            }

            virtual crimild::Bool encode( std::string key, SharedPointer< coding::Codable > const &codable ) override
            {
                if ( codable == nullptr ) {
                    return false;
                }

                encodeKey( key );

                return true;
            }

            virtual crimild::Bool encode( std::string key, std::string value ) override
            {
                encodeKey( key );
                return true;
            }

            virtual crimild::Bool encode( std::string key, crimild::Size value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt8 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt16 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Int16 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Int32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Bool value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Real32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Real64 value ) override { return encodeValue( key, value ); }

            virtual crimild::Bool encode( std::string key, const ColorRGB &value ) override
            {
                // TODO
                return false;
            }

            virtual crimild::Bool encode( std::string key, const ColorRGBA &value ) override
            {
                // TODO
                return false;
            }

            virtual crimild::Bool encode( std::string key, const Vector2f &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector3f &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector4f &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Matrix3f &value ) override { return encodeValues( key, 9, static_cast< const float * >( &value.c0.x ) ); }
            virtual crimild::Bool encode( std::string key, const Matrix4f &value ) override { return encodeValues( key, 16, static_cast< const float * >( &value[ 0 ].x ) ); }
            virtual crimild::Bool encode( std::string key, const Quaternion &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.getRawData().x ) ); }

            virtual crimild::Bool encode( std::string key, const Transformation &value ) override
            {
                return true;
            }

            virtual crimild::Bool encode( std::string key, ByteArray &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< crimild::Real32 > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Vector3f > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Vector4f > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Matrix3f > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Matrix4f > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Quaternion > &value ) override { return false; }

        protected:
            virtual void encodeArrayBegin( std::string key, crimild::Size count ) override
            {
                // TODO
            }

            virtual std::string beginEncodingArrayElement( std::string key, crimild::Size index ) override
            {
                return key;
            }

            virtual void endEncodingArrayElement( std::string key, crimild::Size index ) override
            {
                // TODO
            }

            virtual void encodeArrayEnd( std::string key ) override
            {
                // TODO
            }

        private:
            template< typename T >
            crimild::Bool encodeValue( std::string key, const T &value )
            {
                encodeKey( key );
                return true;
            }

            template< typename T >
            crimild::Bool encodeValues( std::string key, crimild::Size count, const T *values )
            {
                encodeKey( key );
                return true;
            }

            crimild::Bool encodeKey( std::string key )
            {
                // TODO
                return true;
            }
        };

    }

}

TEST( Codable, mutable_coding )
{
    CRIMILD_REGISTER_OBJECT_BUILDER( crimild::CodableNode )

    auto n = crimild::alloc< crimild::CodableNode >( "a scene" );

    auto coder = crimild::alloc< coding::MutableMemoryCoder >();

    // TODO

    // coder->encode( n );
    // coder->set( n->getUniqueID(), "name", "some other name" );
    // coder->apply();

    // EXPECT_EQ( "some other name", n->getName() );
}
