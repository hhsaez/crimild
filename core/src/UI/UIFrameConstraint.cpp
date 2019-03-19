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
#include "UICanvas.hpp"

#include "SceneGraph/Node.hpp"
#include "Concurrency/Async.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/Apply.hpp"

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

void UIFrameConstraint::apply( UIFrame *frame, UIFrame *parentFrame )
{
    if ( _referenceFrame == nullptr && _referenceFrameName != "" ) {
        // TODO: not sure about this. But its better than doing it on apply
        auto scene = Simulation::getInstance()->getScene();
        scene->perform( Apply( [ this ]( Node *n ) {
            if ( n->getName() == _referenceFrameName ) {
                _referenceFrame = n->getComponent< UIFrame >();
            }
        }));
    }


	auto rect = frame->getExtensions();
	auto z = frame->getZIndex();

	auto ref = _referenceFrame != nullptr ? _referenceFrame->getExtensions() : parentFrame->getExtensions();
	auto refZ = _referenceFrame != nullptr ? _referenceFrame->getZIndex() : parentFrame->getZIndex();

	crimild::Real32 x = rect.getX();
	crimild::Real32 y = rect.getY();
	crimild::Real32 w = rect.getWidth();
	crimild::Real32 h = rect.getHeight();

	auto rx = ref.getX();
	auto ry = ref.getY();
	auto rw = ref.getWidth();
	auto rh = ref.getHeight();

	auto v = _value;

	switch ( _type ) {
		case Type::WIDTH:
			w = _value;
			break;

		case Type::WIDTH_TO_PARENT:
			w = _value / 100.0f * ref.getWidth();
			break;

		case Type::HEIGHT:
			h = _value;
			break;

		case Type::HEIGHT_TO_PARENT:
			h = _value / 100.0f * ref.getHeight();
			break;

		case Type::TOP:
			y = _value;
			break;

		case Type::LEFT:
			x = _value;
			break;

		case Type::RIGHT:
			if ( x == 0 && w != rw ) {
				// width was modified. apply offset, but keep width value
				x = rw - w - v;
			}
			else {
				// left edge was modified. so, this changes right edge
				w = rw - x - v;
			}
			break;

		case Type::BOTTOM:
			if ( y == 0 && h != rw ) {
				// height was modified. apply offset, but keep height value
				y = rh - h - v;
			}
			else {
				// top edge was modified. Change bottom edge
				h = rh - y - v;
			}
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
			x = 0.5f * ( ref.getWidth() - w );
			break;

		case Type::CENTER_Y:
			y = 0.5f * ( ref.getHeight() - h );
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

void UIFrameConstraint::decode( coding::Decoder &decoder )
{
	coding::Codable::decode( decoder );

	crimild::Int32 constraintType;
	decoder.decode( "constraintType", constraintType );
	_type = static_cast< Type >( constraintType );

	decoder.decode( "value", _value );
	decoder.decode( "reference", _referenceFrameName );
}

