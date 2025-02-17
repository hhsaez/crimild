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

#include "Visitors/VulkanFetchSceneRenderState.hpp"

#include "Components/MaterialComponent.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/UnlitMaterial.hpp"
#include "SceneGraph/Geometry.hpp"

using namespace crimild;
using namespace crimild::vulkan;

void FetchSceneRenderState::traverse( Node *node ) noexcept
{
    m_result = Result {};
    NodeVisitor::traverse( node );
}

void FetchSceneRenderState::visitGeometry( Geometry *geometry ) noexcept
{
    if ( geometry == nullptr ) {
        return;
    }

    auto materials = geometry->getComponent< MaterialComponent >();
    if ( materials == nullptr ) {
        return;
    }

    auto material = crimild::retain( materials->first() );
    if ( material == nullptr ) {
        return;
    }

    auto primitive = crimild::retain( geometry->anyPrimitive() );
    if ( primitive == nullptr ) {
        return;
    }

    if ( geometry->getLayer() == Node::Layer::SKYBOX ) {
        if ( auto m = std::dynamic_pointer_cast< UnlitMaterial >( material ) ) {
            m_result.envRenderables[ m ][ primitive ].push_back( { Matrix4( geometry->getWorld() ) } );
        }
        return;
    }

    // TODO: replace dynamic casts with render mode for materials
    if ( auto m = std::dynamic_pointer_cast< materials::PrincipledBSDF >( material ) ) {
        m_result.litRenderables[ m ][ primitive ].push_back( { Matrix4( geometry->getWorld() ) } );
        m_result.shadowCasters[ primitive ].push_back( { Matrix4( geometry->getWorld() ) } );
    } else if ( auto m = std::dynamic_pointer_cast< UnlitMaterial >( material ) ) {
        m_result.unlitRenderables[ m ][ primitive ].push_back( { Matrix4( geometry->getWorld() ) } );
    }
}

void FetchSceneRenderState::visitLight( Light *light ) noexcept
{
    m_result.lights[ light->getType() ].insert( crimild::retain( light ) );
}
