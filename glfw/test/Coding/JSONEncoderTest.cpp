/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Coding/JSONEncoder.hpp"

#include "SceneGraph/Group.hpp"

#include <gtest/gtest.h>

using namespace crimild;
using namespace nlohmann;

TEST( JSONEncoder, encodeVector3 )
{
    coding::JSONEncoder encoder;
    encoder.encode( "v", Vector3 { 1, 2, 3 } );
    const auto res = encoder.getResult();
    const auto expected = json { { "v", { 1.0, 2.0, 3.0 } } };
    EXPECT_EQ( expected, res );
}

TEST( JSONEncoder, encodeGroup )
{
    auto group = crimild::alloc< Group >();
    group->setName( "a group" );

    const auto expected = json {
        { "__CLASS_NAME", "crimild::Group" },
        { "__id", group->getUniqueID() },
        { "layer", 0 },
        { "name", "a group" },
        { "worldIsCurrent", false },
    };

    coding::JSONEncoder encoder;
    encoder.encode( group );
    const auto res = encoder.getResult();
    EXPECT_EQ( expected, res );
}

TEST( JSONEncoder, encodes_group_with_child_nodes )
{
    auto group = crimild::alloc< Group >();
    group->setName( "a group" );
    group->attachNode( crimild::alloc< Group >( "a child" ) );

    const auto expected = json {
        { "__CLASS_NAME", "crimild::Group" },
        { "__id", group->getUniqueID() },
        { "layer", 0 },
        { "name", "a group" },
        { "worldIsCurrent", false },
        { "nodes", {} },
    };

    coding::JSONEncoder encoder;
    encoder.encode( group );
    const auto res = encoder.getResult();
    EXPECT_EQ( expected, res );
}
