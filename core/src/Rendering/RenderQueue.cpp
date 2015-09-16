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

#include "Rendering/RenderQueue.hpp"

using namespace crimild;

RenderQueue::RenderQueue( void )
{
    setTimestamp( std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch() ).count() );
}

RenderQueue::~RenderQueue( void )
{
    
}

void RenderQueue::reset( void )
{
    setCamera( nullptr );
    
    _lights.clear();
    _opaqueObjects.clear();
    _translucentObjects.clear();
    _screenObjects.clear();
}

void RenderQueue::setCamera( Camera *camera )
{
    _camera = camera;
    if ( _camera != nullptr ) {
        _viewMatrix = _camera->getViewMatrix();
        _projectionMatrix = _camera->getProjectionMatrix();
    }
    else {
        _viewMatrix.makeIdentity();
        _projectionMatrix.makeIdentity();
    }
}

void RenderQueue::push( Material *material, Primitive *primitive, Geometry *geometry, const Transformation &world, bool renderOnScreen )
{
    if ( renderOnScreen ) {
        _screenObjects[ material ][ primitive ].push_back( std::make_pair( geometry, world.computeModelMatrix() ) );
    }
    else if ( material->getAlphaState()->isEnabled() || material->getProgram() != nullptr ) {
        _translucentObjects[ material ][ primitive ].push_back( std::make_pair( geometry, world.computeModelMatrix() ) );
    }
    else {
        if ( material->castShadows() ) {
            _shadowCasters[ material ][ primitive ].push_back( std::make_pair( geometry, world.computeModelMatrix() ) );
        }
        
        if ( material->receiveShadows() ) {
            _shadedObjects[ material ][ primitive ].push_back( std::make_pair( geometry, world.computeModelMatrix() ) );
        }
        else {
            _opaqueObjects[ material ][ primitive ].push_back( std::make_pair( geometry, world.computeModelMatrix() ) );
        }
    }
}

void RenderQueue::push( Light *light )
{
    _lights.push_back( light );
}

void RenderQueue::each( Renderables const &objects, std::function< void( Material *, PrimitiveMap const & ) > callback )
{
    auto os = objects;
	for ( auto it : os ) {
		callback( it.first, it.second );
	}
}

void RenderQueue::each( std::function< void ( Light *, int ) > callback )
{
    auto lights = _lights;
    int i = 0;
    for ( auto l : lights ) {
        callback( l, i++ );
    }
}

