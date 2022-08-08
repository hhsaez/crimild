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

#include "Components/PathComponent.hpp"

#include "Debug/DebugDrawManager.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/ceil.hpp"
#include "Mathematics/easing.hpp"
#include "Mathematics/floor.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/min.hpp"
#include "SceneGraph/Group.hpp"

using namespace crimild;
using namespace crimild::components;

Transformation Path::evaluate( float time ) noexcept
{
    auto group = getNode< Group >();
    if ( !group->hasNodes() ) {
        return Transformation::Constants::IDENTITY;
    }

    const size_t N = group->getNodeCount() - 1;
    const size_t idx0 = max( size_t( 0 ), min( size_t( floor( time * N ) ), N ) );
    const size_t idx1 = max( size_t( 0 ), min( size_t( ceil( time * N ) ), N ) );

    const auto &t0 = group->getNodeAt( idx0 )->getWorld();
    const auto &t1 = group->getNodeAt( idx1 )->getWorld();
    const auto t = ( time * N ) - floor( time * N );

    return Transformation {
        lerp( t0.mat, t1.mat, t ),
        lerp( t0.invMat, t1.invMat, t ),
    };
}

void Path::renderDebugInfo( Renderer *, Camera * )
{
    auto group = getNode< Group >();
    const auto N = group->getNodeCount();
    for ( auto i = 1; i < N; i++ ) {
        const auto p0 = location( group->getNodeAt( i )->getWorld() );
        const auto p1 = location( group->getNodeAt( i - 1 )->getWorld() );
        DebugDrawManager::addLine( p0, p1, ColorRGB { 1, 0, 1 } );
    }
}
