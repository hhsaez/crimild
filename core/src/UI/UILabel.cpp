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
#include "Rendering/Font.hpp"
#include "SceneGraph/Text.hpp"
#include "Simulation/AssetManager.hpp"
#include "Boundings/Box2DBoundingVolume.hpp"
#include "Debug/DebugRenderHelper.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"

using namespace crimild;
using namespace crimild::ui;

UILabel::UILabel( void )
	: UILabel( "asd" )
{

}

UILabel::UILabel( std::string str, const RGBAColorf &color )
	: _textColor( color )
{
    auto font = AssetManager::getInstance()->get< Font >( AssetManager::FONT_DEFAULT );

    _text = crimild::alloc< Text >();
    _text->setFont( font );
    _text->setSize( 1.0f );
    _text->setText( str );
    _text->setTextColor( _textColor );
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
	auto scale = rect.getHeight();
    auto geo = _text->getNodeAt( 0 );
	auto bounds = geo->getLocalBound();
    auto center = bounds->getCenter();

    auto x = 0.0f;
    auto y = 0.0f;

    switch ( _textHorizontalAlignment ) {
		case TextHorizontalAlignment::Left:
            x = -0.5f * rect.getWidth();
			break;

		case TextHorizontalAlignment::Center:
			x = -_textSize * center.x();
			break;

		case TextHorizontalAlignment::Right:
            x = 0.5f * rect.getWidth() - _textSize * ( bounds->getMax().x() - bounds->getMin().x() );
			break;
    }

    switch ( _textVerticalAlignment ) {
		case TextVerticalAlignment::Top:
            y = 0.5f * rect.getHeight() - _textSize * ( bounds->getMax().y() - bounds->getMin().y() );
			break;

		case TextVerticalAlignment::Center:
			y = -_textSize * center.y();
			break;

		case TextVerticalAlignment::Bottom:
            y = -0.5f * rect.getHeight() - _textSize * bounds->getMin().y();
			break;
    }

	_text->setTextColor( _textColor );
    _text->local().setTranslate( x, y, 0.05f );
    _text->local().setScale( _textSize );
}

void UILabel::setText( std::string text )
{
	_text->setText( text );
}

void UILabel::decode( coding::Decoder &decoder )
{
	NodeComponent::decode( decoder );

	std::string text;
	decoder.decode( "text", text );
	setText( text );

    _textColor = RGBAColorf::ONE;
	decoder.decode( "textColor", _textColor );

    _textSize = 10;
    decoder.decode( "textSize", _textSize );

	auto textHorizontalAlignment = static_cast< int >( TextHorizontalAlignment::Left );
	decoder.decode( "textHorizontalAlignment", textHorizontalAlignment );
	_textHorizontalAlignment = static_cast< TextHorizontalAlignment >( textHorizontalAlignment );

	auto textVerticalAlignment = static_cast< int >( TextVerticalAlignment::Center );
	decoder.decode( "textVerticalAlignment", textVerticalAlignment );
	_textVerticalAlignment = static_cast< TextVerticalAlignment >( textVerticalAlignment );
}
