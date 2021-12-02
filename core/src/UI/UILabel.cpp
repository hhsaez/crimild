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

#include "UILabel.hpp"
#include "UIFrame.hpp"

#include "Components/UIResponder.hpp"
#include "SceneGraph/Text.hpp"
#include "Simulation/AssetManager.hpp"
#include "Boundings/Box2DBoundingVolume.hpp"

using namespace crimild;
using namespace crimild::ui;

UILabel::UILabel( void )
	: UILabel( "Label" )
{

}

UILabel::UILabel( std::string str, const RGBAColorf &color )
{
    auto font = AssetManager::getInstance()->get< Font >( AssetManager::FONT_DEFAULT );
	
    _text = crimild::alloc< Text >();
    _text->setFont( font );
    _text->setSize( 1.0f );
    _text->setText( str );
    _text->setTextColor( color );
}

UILabel::~UILabel( void )
{
	
}

void UILabel::onAttach( void )
{
	getNode< Group >()->attachNode( _text );
}

void UILabel::start( void )
{
    _frame = getComponent< UIFrame >();
}

void UILabel::update( const Clock & )
{
    const auto &rect = _frame->getExtensions();
	auto w = rect.getWidth();
	auto h = rect.getHeight();

    _text->local().setTranslate( -0.5f * w, -0.5f * h, 0.0f );
    _text->local().setScale( h );
}

void UILabel::setText( std::string text )
{
	_text->setText( text );
}

