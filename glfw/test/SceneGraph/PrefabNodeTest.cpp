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

#include "SceneGraph/PrefabNode.hpp"

#include "Coding/FileEncoder.hpp"
#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"

#include <gtest/gtest.h>

using namespace crimild;

TEST( PrefabNode, create_an_empty_prefab_from_invalid_file )
{
    auto prefab = crimild::alloc< PrefabNode >( "path/to/my/prefab" );
    EXPECT_EQ( 0, prefab->getNodeCount() );
}

TEST( PrefabNode, create_a_prefab_from_nodes )
{
    const auto path = std::filesystem::current_path() / std::string( "prefab.crimild" );

    {
        auto node = crimild::alloc< Node >( "a node" );
        coding::FileEncoder encoder;
        encoder.encode( node );
        ASSERT_TRUE( encoder.write( path ) );
    }

    auto prefab = crimild::alloc< PrefabNode >( path );
    ASSERT_TRUE( prefab->hasNodes() );
    EXPECT_STREQ( "a node", prefab->getNodeAt( 0 )->getName().c_str() );
}
