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

#include "UIFrameConstraint.hpp"
#include "UIFrame.hpp"

#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace ui;

UIFrameConstraint::UIFrameConstraint( Type type, crimild::Real32 value )
	: _type( type ),
	  _value( value )
{
	
}

UIFrameConstraint::UIFrameConstraint( Type type, UIFrame *referenceFrame )
	: _type( type ),
	  _referenceFrame( referenceFrame )
{
	
}

UIFrameConstraint::~UIFrameConstraint( void )
{

}

void UIFrameConstraint::apply( UIFrame *frame, UIFrame *parentFrame ) const
{
	auto rect = frame->getExtensions();
	auto z = frame->getZIndex();

	auto ref = _referenceFrame != nullptr ? _referenceFrame->getExtensions() : parentFrame->getExtensions();
	auto refZ = _referenceFrame != nullptr ? _referenceFrame->getZIndex() : parentFrame->getZIndex();

	crimild::Real32 x = rect.getX();
	crimild::Real32 y = rect.getY();
	crimild::Real32 w = rect.getWidth();
	crimild::Real32 h = rect.getHeight();

	switch ( _type ) {
		case Type::TOP:
			y = ref.getY() + _value;
			break;

		case Type::LEFT:
			x = ref.getX() + _value;
			break;

		case Type::RIGHT:
			if ( _referenceFrame == nullptr ) {
				x = ref.getX() + ref.getWidth() - w - _value;
			}
			else {
				w = ref.getX() + ref.getWidth() - x;
			}
			break;

		case Type::BOTTOM:
			if ( _referenceFrame == nullptr ) {
				y = ref.getY() + ref.getHeight() - h - _value;
			}
			else {
				h = ref.getY() + ref.getHeight() - y;
			}
			break;

		case Type::WIDTH:
			w = _value;
			break;

		case Type::HEIGHT:
			h = _value;
			break;

		case Type::EDGES:
			x = 0;
			y = 0;
			w = ref.getWidth();
			h = ref.getHeight();
			break;

		case Type::CENTER:
			break;

		case Type::CENTER_X:
			x = ref.getX() + 0.5f * ( ref.getWidth() - w );
			break;

		case Type::CENTER_Y:
			y = ref.getY() + 0.5f * ( ref.getHeight() - h );
			break;

		case Type::AFTER:
			x = ref.getX() + ref.getWidth();
			break;

		case Type::BELOW:
			y = ref.getY() + ref.getHeight();
			break;

		case Type::MARGIN:
			x += _value;
			y += _value;
			w -= 2 * _value;
			h -= 2 * _value;
			break;

		case Type::MARGIN_TOP:
			y += _value;
			break;

		case Type::MARGIN_RIGHT:
			w -= _value;
			break;

		case Type::MARGIN_BOTTOM:
			h -= _value;
			break;

		case Type::MARGIN_LEFT:
			x += _value;
			break;

		case Type::BEHIND:
			z = refZ - 0.001f;
			break;

		case Type::INFRONT:
			z = refZ + 0.001f;
			break;

		default:
			break;
	}

	frame->setExtensions( Rectf( x, y, w, h ) );
	frame->setZIndex( z );
}

