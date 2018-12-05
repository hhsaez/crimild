/*
 * Copyright (c) 2002-present, H. Hernan Saez
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
#include "Primitives/Primitive.hpp"
#include "Components/RenderStateComponent.hpp"

using namespace crimild;

RenderQueue::RenderQueue( void )
{
    setTimestamp( ( unsigned long ) std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch() ).count() );
}

RenderQueue::~RenderQueue( void )
{
    reset();
}

void RenderQueue::reset( void )
{
    setCamera( nullptr );

    _lights.clear();

    for ( auto &it : _renderables ) {
        it.second.clear();
    }
    _renderables.clear();
}

void RenderQueue::setCamera( Camera *camera )
{
    if ( camera != nullptr ) {
        _camera = crimild::retain( camera );
        _viewMatrix = _camera->getViewMatrix();
        _projectionMatrix = _camera->getProjectionMatrix();
    }
    else {
        _camera = nullptr;
        _viewMatrix.makeIdentity();
        _projectionMatrix.makeIdentity();
    }
}

void RenderQueue::push( Geometry *geometry )
{
    auto rs = geometry->getComponent< RenderStateComponent >();
    if ( rs == nullptr ) {
        return;
    }
    
    bool renderOnScreen = rs->renderOnScreen();
    
    rs->forEachMaterial( [this, geometry, renderOnScreen]( Material *material ) {
        auto renderableType = RenderQueue::RenderableType::OPAQUE;
        bool castShadows = false;
        
        if ( renderOnScreen ) {
            renderableType = RenderQueue::RenderableType::SCREEN;
        }
        else if ( material->getColorMaskState()->isEnabled() &&
                  ( !material->getColorMaskState()->getRMask() ||
                    !material->getColorMaskState()->getGMask() ||
                    !material->getColorMaskState()->getBMask() ||
                    !material->getColorMaskState()->getAMask() ) ) {
            // if at least one of the color masks is disabled, then
            // the object is considered as an occluder
            renderableType = RenderQueue::RenderableType::OCCLUDER;
        }
        else if ( material->getAlphaState()->isEnabled() ) {
			if ( material->getProgram() != nullptr ) {
				renderableType = RenderQueue::RenderableType::TRANSLUCENT_CUSTOM;
			}
			else {
				renderableType = RenderQueue::RenderableType::TRANSLUCENT;
			}
        }
        else {
            // only opaque objects cast shadows
            castShadows = material->castShadows();
			if ( material->getProgram() != nullptr ) {
				renderableType = RenderQueue::RenderableType::OPAQUE_CUSTOM;
			}
			else {
				renderableType = RenderQueue::RenderableType::OPAQUE;
			}
        }
        
        auto renderable = RenderQueue::Renderable {
            crimild::retain( geometry ),
            crimild::retain( material ),
            
            geometry->getWorld().computeModelMatrix(),
            
            // we use the squared distance to avoid performance penalties
            Distance::computeSquared( geometry->getWorld().getTranslate(), getCamera()->getWorld().getTranslate() ),
        };
        
        auto queue = &_renderables[ renderableType ];
        
        if ( renderableType == RenderQueue::RenderableType::TRANSLUCENT ||
		    renderableType == RenderQueue::RenderableType::TRANSLUCENT_CUSTOM ) {
            // order BACK_TO_FRONT for translucent and screen objects
            auto it = queue->begin();
            while ( it != queue->end() && ( *it ).distanceFromCamera >= renderable.distanceFromCamera ) {
                it++;
            }
            queue->insert( it, renderable );
        }
        else if ( renderableType != RenderQueue::RenderableType::SCREEN ) {
            // order FRONT_TO_BACK for everything else, except SCREEN
            auto it = queue->begin();
            while ( it != queue->end() && ( *it ).distanceFromCamera <= renderable.distanceFromCamera ) {
                it++;
            }
            queue->insert( it, renderable );
        } else {
			queue->push_back( renderable );
		}
        
        if ( castShadows ) {
            // if the geometry is supposed to cast shadows, we also add it to that queue
            // order FRONT_TO_BACK
            auto casters = &_renderables[ RenderQueue::RenderableType::SHADOW_CASTER ];
            auto it = casters->begin();
            while ( it != casters->end() && ( *it ).distanceFromCamera <= renderable.distanceFromCamera ) {
                it++;
            }
            casters->insert( it, renderable );
        }
    });
}

void RenderQueue::push( Light *light )
{
    _lights.push_back( crimild::retain( light ) );
}

void RenderQueue::each( Renderables *renderables, std::function< void( Renderable * ) > callback )
{
    for ( auto &r : *renderables ) {
        callback( &r );
    }
}

void RenderQueue::each( std::function< void ( Light *, int ) > callback )
{
    auto lights = _lights;
    int i = 0;
    for ( auto l : lights ) {
        if ( l->isEnabled() ) {
            callback( crimild::get_ptr( l ), i++ );
        }
    }
}

