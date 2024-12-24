#include "Rendering/VertexAttribute.hpp"

#include <gtest/gtest.h>

using namespace crimild;

TEST( VertexAttribute, DefaultConstructor )
{
    VertexAttribute attr;
    ASSERT_EQ( attr.getName(), VertexAttribute::Name::UNDEFINED );
    ASSERT_EQ( attr.getFormat(), Format::UNDEFINED );
    ASSERT_EQ( attr.getOffset(), 0 );
}

TEST( VertexAttribute, ParameterizedConstructor )
{
    VertexAttribute attr( VertexAttribute::Name::NORMAL, Format::R32G32B32_SFLOAT, 12 );
    ASSERT_EQ( attr.getName(), VertexAttribute::Name::NORMAL );
    ASSERT_EQ( attr.getFormat(), Format::R32G32B32_SFLOAT );
    ASSERT_EQ( attr.getOffset(), 12 );
}

TEST( VertexAttribute, SettersAndGetters )
{
    VertexAttribute attr;
    attr.setName( VertexAttribute::Name::COLOR );
    attr.setFormat( Format::R32G32B32A32_SFLOAT );
    attr.setOffset( 24 );

    ASSERT_EQ( attr.getName(), VertexAttribute::Name::COLOR );
    ASSERT_EQ( attr.getFormat(), Format::R32G32B32A32_SFLOAT );
    ASSERT_EQ( attr.getOffset(), 24 );
}

TEST( VertexAttribute, EqualityOperator )
{
    VertexAttribute attr1( VertexAttribute::Name::TEX_COORD, Format::R32G32_SFLOAT, 8 );
    VertexAttribute attr2( VertexAttribute::Name::TEX_COORD, Format::R32G32_SFLOAT, 8 );
    VertexAttribute attr3( VertexAttribute::Name::POSITION, Format::R32G32_SFLOAT, 8 );

    ASSERT_TRUE( attr1 == attr2 );
    ASSERT_FALSE( attr1 == attr3 );
}

TEST( VertexAttribute, EncodeDecode )
{
    auto attr = crimild::alloc< VertexAttribute >( VertexAttribute::Name::BLEND_WEIGHT, Format::R32G32B32A32_SFLOAT, 16 );

    coding::MemoryEncoder encoder;
    encoder.encode( attr );

    coding::MemoryDecoder decoder;
    decoder.fromBytes( encoder.getBytes() );
    ASSERT_TRUE( decoder.getObjectCount() > 0 );

    auto decodedAttr = decoder.getObjectAt< VertexAttribute >( 0 );
    EXPECT_EQ( attr->getName(), decodedAttr->getName() );
    EXPECT_EQ( attr->getFormat(), decodedAttr->getFormat() );
    EXPECT_EQ( attr->getOffset(), decodedAttr->getOffset() );
}
