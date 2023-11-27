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

#include "UIFrame.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Mathematics/Vector4.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Node.hpp"
#include "UIFrameConstraint.hpp"
#include "UIFrameConstraintMaker.hpp"

using namespace crimild;
using namespace crimild::ui;

UIFrame::UIFrame( void )
    : UIFrame( Rectf { { 0.0f, 0.0f }, { 1.0f, 1.0f } } )
{
}

UIFrame::UIFrame( const Rectf &extensions )
    : _extensions( extensions ),
      _constraintMaker( this )
{
}

void UIFrame::start( void )
{
    NodeComponent::start();

    // sort constraints by priority/type
    _constraints.sort( []( const SharedPointer< UIFrameConstraint > &lhs, const SharedPointer< UIFrameConstraint > &rhs ) {
        return static_cast< int >( lhs->getType() ) < static_cast< int >( rhs->getType() );
    } );
}

void UIFrame::update( const Clock & )
{
    UIFrame *parentFrame = nullptr;
    if ( auto parent = getNode()->getParent() ) {
        parentFrame = parent->getComponent< UIFrame >();
    }

    if ( auto pf = parentFrame ) {
        _extensions = Rectf { { 0, 0 }, { pf->getExtensions().size.width, pf->getExtensions().size.height } };
    }

    _constraints.each( [ this, parentFrame ]( SharedPointer< UIFrameConstraint > const &c ) {
        c->apply( this, parentFrame );
    } );

    if ( parentFrame != nullptr ) {
        /*
                const auto &frame = getExtensions();
                auto w = frame.getWidth();
                auto h = frame.getHeight();
                auto pExtensions = parentFrame->getExtensions();
                auto pW = pExtensions.getWidth();
                auto pH = pExtensions.getHeight();
                auto x = frame.getX() + 0.5f * ( -pW + w );
                auto y = -frame.getY() + 0.5f * ( pH - h );
                getNode()->local().setTranslate( Vector3f( x, y, 0.01f + _zIndex ) );
        */
    }
}

UIFrame *UIFrame::clearConstraints( void )
{
    _constraints.clear();
    return this;
}

UIFrame *UIFrame::addConstraint( SharedPointer< UIFrameConstraint > const &constraint )
{
    _constraints.add( constraint );
    return this;
}

UIFrameConstraint *UIFrame::getConstraint( UIFrameConstraint::Type type )
{
    UIFrameConstraint *ret = nullptr;

    _constraints.each( [ &ret, type ]( SharedPointer< UIFrameConstraint > const &c ) {
        if ( c->getType() == type ) {
            ret = crimild::get_ptr( c );
        }
    } );

    return ret;
}

UIFrameConstraintMaker *UIFrame::pin( void )
{
    clearConstraints();
    return &_constraintMaker;
}

void UIFrame::decode( coding::Decoder &decoder )
{
    NodeComponent::decode( decoder );

    auto frame = Vector4f::Constants::ZERO;
    decoder.decode( "extensions", frame );
    _extensions = Rectf { frame.x, frame.y, frame.z, frame.w };

    decoder.decode( "constraints", _constraints );
}
