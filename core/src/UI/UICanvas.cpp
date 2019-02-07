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
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;
using namespace crimild::ui;

UICanvas::UICanvas( crimild::Int32 width, crimild::Int32 height )
	: _size( width, height )
{
	
}

UICanvas::~UICanvas( void )
{
	
}

void UICanvas::onAttach( void )
{
	NodeComponent::onAttach();
	
	if ( getComponent< UIFrame >() == nullptr ) {
		getNode()->attachComponent< UIFrame >( Rectf( 0, 0, _size.x(), _size.y() ) );
	}
}

void UICanvas::update( const Clock & )
{
	auto node = getNode();
	node->setWorldIsCurrent( false );
	
	if ( getRenderSpace() == RenderSpace::CAMERA ) {
        if ( auto camera = Camera::getMainCamera() ) {
            auto cWorld = camera->getWorld();
            auto t = Transformation();
            auto size = Numericf::max( _size.x(), _size.y() );
            t.setScale( 1.0f / size );
            t.setTranslate( 0.0f, 0.0f, -1.0f );
            node->world().computeFrom( cWorld, t );
            node->setWorldIsCurrent( true );
        }
    }
}

