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

#include "UpdateRenderState.hpp"

#include "Animation/Skeleton.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Components/SkinnedMeshComponent.hpp"
#include "FetchLights.hpp"
#include "Rendering/Material.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Light.hpp"

using namespace crimild;
using namespace crimild::animation;

UpdateRenderState::UpdateRenderState( void )
{
    setDefaultMaterial( crimild::alloc< Material >() );
}

UpdateRenderState::~UpdateRenderState( void )
{
}

void UpdateRenderState::traverse( Node *node )
{
    _lights.clear();

    FetchLights fetchLights;
    if ( node->hasParent() ) {
        node->getRootParent()->perform( fetchLights );
    } else {
        node->perform( fetchLights );
    }

    auto self = this;
    fetchLights.forEachLight( [ self ]( Light *light ) mutable {
        self->_lights.push_back( light );
    } );

    NodeVisitor::traverse( node );
}

void UpdateRenderState::visitGroup( Group *group )
{
    auto tempSkeleton = _skeleton;

    if ( auto skeleton = group->getComponent< Skeleton >() ) {
        _skeleton = skeleton;
    }

    NodeVisitor::visitGroup( group );

    _skeleton = tempSkeleton;
}

void UpdateRenderState::visitGeometry( Geometry *geometry )
{
    auto rs = geometry->getComponent< RenderStateComponent >();

    rs->detachAllMaterials();
    auto materials = geometry->getComponent< MaterialComponent >();
    if ( materials->hasMaterials() ) {
        materials->forEachMaterial( [ rs ]( Material *material ) mutable {
            rs->attachMaterial( material );
        } );
    } else {
        rs->attachMaterial( _defaultMaterial );
    }

    rs->detachAllLights();
    for ( auto light : _lights ) {
        rs->attachLight( light );
    }

    if ( auto skeleton = geometry->getComponent< Skeleton >() ) {
        rs->setSkeleton( crimild::retain( skeleton ) );
    } else if ( _skeleton != nullptr ) {
        rs->setSkeleton( crimild::retain( _skeleton ) );
    }
}

void UpdateRenderState::visitCSGNode( CSGNode *csg )
{
    NodeVisitor::visitCSGNode( csg );
}
