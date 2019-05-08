/*
 * Copyright (c) 2002-present, H. Hernán Saez
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

#include "UICanvas.hpp"
#include "UIFrame.hpp"

#include "SceneGraph/Group.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;
using namespace crimild::ui;

UICanvas::UICanvas( crimild::Int32 width, crimild::Int32 height )
	: _size( width, height ),
      _safeArea( 0, 0 )
{
	
}

void UICanvas::onAttach( void )
{
	NodeComponent::onAttach();
	
	if ( getComponent< UIFrame >() == nullptr ) {
		getNode()->attachComponent< UIFrame >( Rectf( 0, 0, _size.x() - _safeArea.x(), _size.y() - _safeArea.y() ) );
	}
}

void UICanvas::update( const Clock & )
{
	auto node = getNode();
	node->setWorldIsCurrent( false );
	
	if ( getRenderSpace() == RenderSpace::CAMERA ) {
        if ( auto camera = Camera::getMainCamera() ) {
            // Scale the UI so its height is 1.0
            // Then, translate it back based on Near/Up planes in camera's frustum
            auto cWorld = camera->getWorld();
            auto t = Transformation();
            auto size = _size.y();
            auto up = camera->getFrustum().getUMax();
            auto near = camera->getFrustum().getDMin();
            t.setScale( 2.0f / size );
            t.setTranslate( 0.0f, 0.0f, -near / up );
            node->setLocal( t );
        }
    }
}

void UICanvas::decode( coding::Decoder &decoder )
{
	NodeComponent::decode( decoder );

	// Grab canvas size from settings first
	auto settings = Simulation::getInstance()->getSettings();
	auto width = settings->get< crimild::Int32 >( "video.width", 800 );
	auto height = settings->get< crimild::Int32 >( "video.height", 600 );
    auto safeArea = settings->get< crimild::Int32 >( "video.safeArea", 0 );

    _safeArea = Vector2i { 0, safeArea };

	decoder.decode( "width", width );
	decoder.decode( "height", height );

	_size = Vector2i { width, height };

	int renderSpace;
	decoder.decode( "renderSpace", renderSpace );
	_renderSpace = static_cast< RenderSpace >( renderSpace );
}

