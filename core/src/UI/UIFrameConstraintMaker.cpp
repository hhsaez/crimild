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

#include "UIFrameConstraintMaker.hpp"
#include "UIFrame.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;
using namespace crimild::ui;

UIFrameConstraintMaker::UIFrameConstraintMaker( UIFrame *frame )
	: _frame( frame )
{
	
}

UIFrameConstraintMaker::~UIFrameConstraintMaker( void )
{

}

UIFrameConstraintMaker *UIFrameConstraintMaker::addConstraint( UIFrameConstraint::Type type, crimild::Real32 value )
{
	_frame->addConstraint( crimild::alloc< UIFrameConstraint >( type, value ) );
	return this;
}

UIFrameConstraintMaker *UIFrameConstraintMaker::addConstraint( UIFrameConstraint::Type type, SharedPointer< Node > const &other )
{
	_frame->addConstraint( crimild::alloc< UIFrameConstraint >( type, other->getComponent< UIFrame >() ) );
	return this;
}

UIFrameConstraintMaker *UIFrameConstraintMaker::top( crimild::Real32 offset )
{
	return addConstraint( UIFrameConstraint::Type::TOP, offset );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::top( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::TOP, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::left( crimild::Real32 offset )
{
	return addConstraint( UIFrameConstraint::Type::LEFT, offset );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::left( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::LEFT, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::right( crimild::Real32 offset )
{
	return addConstraint( UIFrameConstraint::Type::RIGHT, offset );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::right( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::RIGHT, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::bottom( crimild::Real32 offset )
{
	return addConstraint( UIFrameConstraint::Type::BOTTOM, offset );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::bottom( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::BOTTOM, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::width( crimild::Real32 constant )
{
	return addConstraint( UIFrameConstraint::Type::WIDTH, constant );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::widthToParent( crimild::Real32 percent )
{
	return addConstraint( UIFrameConstraint::Type::WIDTH_TO_PARENT, percent );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::height( crimild::Real32 constant )
{
	return addConstraint( UIFrameConstraint::Type::HEIGHT, constant );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::heightToParent( crimild::Real32 percent )
{
	return addConstraint( UIFrameConstraint::Type::HEIGHT_TO_PARENT, percent );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::size( crimild::Real32 width, crimild::Real32 height )
{
	addConstraint( UIFrameConstraint::Type::WIDTH, width );
	addConstraint( UIFrameConstraint::Type::HEIGHT, height );
	return this;
}

UIFrameConstraintMaker *UIFrameConstraintMaker::center( crimild::Real32 offset )
{
	return addConstraint( UIFrameConstraint::Type::CENTER, offset );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::center( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::CENTER, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::centerX( crimild::Real32 offset )
{
	return addConstraint( UIFrameConstraint::Type::CENTER_X, offset );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::centerX( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::CENTER_X, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::centerY( crimild::Real32 offset )
{
	return addConstraint( UIFrameConstraint::Type::CENTER_Y, offset );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::centerY( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::CENTER_Y, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::all( void )
{
	return addConstraint( UIFrameConstraint::Type::EDGES, 0 );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::after( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::AFTER, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::before( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::BEFORE, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::above( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::ABOVE, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::below( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::BELOW, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::behindOf( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::BEHIND, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::inFrontOf( SharedPointer< Node > const &other )
{
	return addConstraint( UIFrameConstraint::Type::INFRONT, other );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::margin( crimild::Real32 value )
{
	return addConstraint( UIFrameConstraint::Type::MARGIN, value );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::marginLeft( crimild::Real32 value )
{
	return addConstraint( UIFrameConstraint::Type::MARGIN_LEFT, value );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::marginRight( crimild::Real32 value )
{
	return addConstraint( UIFrameConstraint::Type::MARGIN_RIGHT, value );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::marginTop( crimild::Real32 value )
{
	return addConstraint( UIFrameConstraint::Type::MARGIN_TOP, value );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::marginBottom( crimild::Real32 value )
{
	return addConstraint( UIFrameConstraint::Type::MARGIN_BOTTOM, value );
}

UIFrameConstraintMaker *UIFrameConstraintMaker::margin( crimild::Real32 top, crimild::Real32 right, crimild::Real32 bottom, crimild::Real32 left )
{
	addConstraint( UIFrameConstraint::Type::MARGIN_TOP, top );
	addConstraint( UIFrameConstraint::Type::MARGIN_RIGHT, ( left + right ) );
	addConstraint( UIFrameConstraint::Type::MARGIN_BOTTOM, ( top + bottom ) );
	addConstraint( UIFrameConstraint::Type::MARGIN_LEFT, left );
	return this;
}
