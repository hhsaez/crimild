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

#include "Visitors/ComputeRenderQueue.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Rendering/RenderQueue.hpp"

#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Group.hpp"


using namespace crimild;

ComputeRenderQueue::ComputeRenderQueue( Camera *camera, RenderQueue *result )
    : _camera( camera ),
      _result( result )
{
}

ComputeRenderQueue::~ComputeRenderQueue( void )
{
    
}

void ComputeRenderQueue::traverse( Node *scene )
{
    _result->reset();
    _result->setCamera( _camera );

    if ( _camera != nullptr ) {
        _camera->computeCullingPlanes();
    }

    NodeVisitor::traverse( scene );
}

void ComputeRenderQueue::visitGroup( Group *group )
{
    // we should not discard groups based on culling
    // since there could be lights or other nodes
    // that affect the scene even if they are not visible
    NodeVisitor::visitGroup( group );
}

void ComputeRenderQueue::visitGeometry( Geometry *geometry )
{
	auto culled = false;
	auto cullMode = geometry->getCullMode();
	switch ( cullMode ) {
		case Node::CullMode::NEVER:
			culled = false;
			break;

		case Node::CullMode::ALWAYS:
			culled = true;
			break;
			
		default:
			culled = _camera != nullptr && _camera->culled( geometry->getWorldBound() );
			break;
	}

	if ( !culled ) {
		_result->push( geometry );
	}
}

void ComputeRenderQueue::visitLight( Light *light )
{
    _result->push( light );
}

