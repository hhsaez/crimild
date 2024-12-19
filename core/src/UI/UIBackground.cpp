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

#include "UIBackground.hpp"

#include "Components/MaterialComponent.hpp"
#include "Crimild_Coding.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Rendering/Material.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Simulation/AssetManager.hpp"
#include "UIFrame.hpp"

using namespace crimild;
using namespace crimild::ui;

UIBackground::UIBackground( const ColorRGBA &color )
    : _knownExtensions { { 0, 0 }, { 0, 0 } }
{
    _geometry = crimild::alloc< Geometry >();
    _geometry->setCullMode( Node::CullMode::NEVER );

    _material = crimild::alloc< Material >();
    _material->setDiffuse( color );
    //_material->setDepthState( DepthState::DISABLED );
    _geometry->getComponent< MaterialComponent >()->attachMaterial( _material );
}

void UIBackground::setBackgroundImage( SharedPointer< Texture > const &texture )
{
    _material->setColorMap( texture );
}

void UIBackground::onAttach( void )
{
    getNode< Group >()->attachNode( _geometry );
}

void UIBackground::update( const Clock & )
{
    auto frame = getComponent< UIFrame >()->getExtensions();
    auto w = frame.size.width;
    auto h = frame.size.height;

    if ( _knownExtensions.size.width != w || _knownExtensions.size.height != h ) {
        _geometry->detachAllPrimitives();
        //_geometry->attachPrimitive( crimild::alloc< QuadPrimitive >( w, h, VertexFormat::VF_P3_UV2 ) );

        _knownExtensions = frame;
    }
}

void UIBackground::decode( coding::Decoder &decoder )
{
    NodeComponent::decode( decoder );

    auto color = ColorRGBA::Constants::WHITE;
    decoder.decode( "color", color );
    _material->setDiffuse( color );

    std::string image;
    decoder.decode( "image", image );
    if ( image.size() > 0 ) {
        _material->setColorMap( AssetManager::getInstance()->get< Texture >( image ) );
    }
}
