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

#include "Visitors/UpdateWorldState.hpp"

#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/Vector_equality.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Vertex.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Utils/MockVisitor.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( UpdateWorldStateTest, singleNode )
{
    auto node = crimild::alloc< Node >();

    EXPECT_TRUE( isIdentity( node->getLocal() ) );
    EXPECT_TRUE( isIdentity( node->getWorld() ) );

    node->setLocal( translation( 0, 0, -5 ) );

    EXPECT_FALSE( isIdentity( node->getLocal() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( node->getLocal() ) );
    EXPECT_TRUE( isIdentity( node->getWorld() ) );

    node->perform( UpdateWorldState() );

    EXPECT_FALSE( isIdentity( node->getLocal() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( node->getLocal() ) );
    EXPECT_FALSE( isIdentity( node->getWorld() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( node->getWorld() ) );
}

TEST( UpdateWorldStateTest, geometry )
{
    auto geometry = crimild::alloc< Geometry >();
    geometry->attachPrimitive(
        [] {
            auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
            primitive->setVertexData(
                {
                    [ & ] {
                        return crimild::alloc< VertexBuffer >(
                            VertexP3N3TC2::getLayout(),
                            Array< VertexP3N3TC2 > {
                                VertexP3N3TC2 {
                                    .position = Vector3f { -1.5f, -1.5f, 0.0f },
                                    .normal = Vector3f { 0, 0, 1 },
                                },
                                VertexP3N3TC2 {
                                    .position = Vector3f { 1.5f, -1.5f, 0.0f },
                                    .normal = Vector3f { 0, 0, 1 },
                                },
                                VertexP3N3TC2 {
                                    .position = Vector3f { 1.0f, 1.5f, 0.0f },
                                    .normal = Vector3f { 0, 0, 1 },
                                },
                            } );
                    }(),
                } );
            primitive->setIndices(
                crimild::alloc< IndexBuffer >(
                    Format::INDEX_32_UINT,
                    Array< crimild::UInt32 > {
                        0,
                        1,
                        2,
                    } ) );
            return primitive;
        }() );

    geometry->perform( UpdateWorldState() );

    EXPECT_TRUE( isIdentity( geometry->getLocal() ) );
    EXPECT_TRUE( isIdentity( geometry->getWorld() ) );

    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), geometry->getWorldBound()->getCenter() );
    EXPECT_TRUE( isEqual( Real( 2.12132 ), geometry->getWorldBound()->getRadius() ) );

    geometry->setLocal( translation( 0, 0, -5 ) );
    geometry->perform( UpdateWorldState() );

    EXPECT_FALSE( isIdentity( geometry->getLocal() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( geometry->getLocal() ) );
    EXPECT_FALSE( isIdentity( geometry->getWorld() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( geometry->getWorld() ) );
}

TEST( UpdateWorldStateTest, hierarchy )
{
    auto group1 = crimild::alloc< Group >();
    auto group2 = crimild::alloc< Group >();
    auto geometry1 = crimild::alloc< Geometry >();
    auto geometry2 = crimild::alloc< Geometry >();
    auto geometry3 = crimild::alloc< Geometry >();

    group1->attachNode( group2.get() );
    group1->attachNode( geometry1.get() );

    group2->attachNode( geometry2.get() );
    group2->attachNode( geometry3.get() );

    EXPECT_TRUE( isIdentity( geometry3->getLocal() ) );
    EXPECT_TRUE( isIdentity( geometry3->getWorld() ) );

    group1->setLocal( translation( 0, 0, -5 ) );
    group1->perform( UpdateWorldState() );

    EXPECT_FALSE( isIdentity( group1->getLocal() ) );
    EXPECT_FALSE( isIdentity( group1->getWorld() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( group1->getWorld() ) );

    EXPECT_TRUE( isIdentity( group2->getLocal() ) );
    EXPECT_FALSE( isIdentity( group2->getWorld() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( group2->getWorld() ) );

    EXPECT_TRUE( isIdentity( geometry1->getLocal() ) );
    EXPECT_FALSE( isIdentity( geometry1->getWorld() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( geometry1->getWorld() ) );

    EXPECT_TRUE( isIdentity( geometry2->getLocal() ) );
    EXPECT_FALSE( isIdentity( geometry2->getWorld() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( geometry2->getWorld() ) );

    EXPECT_TRUE( isIdentity( geometry3->getLocal() ) );
    EXPECT_FALSE( isIdentity( geometry3->getWorld() ) );
    EXPECT_EQ( ( Point3 { 0, 0, -5 } ), location( geometry3->getWorld() ) );
}

TEST( UpateWorldStateTest, scale )
{
    auto n0 = crimild::alloc< Group >();
    auto n1 = crimild::alloc< Group >();

    n0->attachNode( n1 );
    n0->setLocal( scale( 0.5f ) );

    EXPECT_FALSE( isIdentity( n0->getLocal() ) );
    EXPECT_TRUE( isIdentity( n1->getLocal() ) );

    n0->perform( UpdateWorldState() );

    EXPECT_EQ( ( Vector3 { 0.5, 0.5, 0.5 } ), n0->getWorld()( Vector3 { 1, 1, 1 } ) );
    EXPECT_EQ( ( Vector3 { 0.5, 0.5, 0.5 } ), n1->getWorld()( Vector3 { 1, 1, 1 } ) );
}

TEST( UpateWorldStateTest, with_csg )
{
    auto n0 = crimild::alloc< Group >();
    auto n1 = crimild::alloc< Group >();
    auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::UNION, n0, n1 );

    csg->setLocal( translation( 1, 0, 0 ) );

    EXPECT_FALSE( isIdentity( csg->getLocal() ) );
    EXPECT_TRUE( isIdentity( n1->getLocal() ) );
    EXPECT_TRUE( isIdentity( n0->getLocal() ) );

    csg->perform( UpdateWorldState() );

    EXPECT_EQ( ( Point3 { 1, 0, 0 } ), location( n0->getWorld() ) );
    EXPECT_EQ( ( Point3 { 1, 0, 0 } ), location( n1->getWorld() ) );
    EXPECT_EQ( ( Point3 { 1, 0, 0 } ), location( csg->getWorld() ) );
}
