/*
 * Copyright (c) 2013, Hernan Saez
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

#include "SceneGraph/CSGNode.hpp"

using namespace crimild;

CSGNode::CSGNode( Operator op ) noexcept
    : m_operator( op )
{
}

CSGNode::CSGNode( Operator op, SharedPointer< Node > const &left, SharedPointer< Node > const &right ) noexcept
    : m_operator( op )
{
    setLeft( left );
    setRight( right );
}

void CSGNode::setLeft( SharedPointer< Node > const &left ) noexcept
{
    if ( m_left ) {
        m_left->setParent( nullptr );
    }
    m_left = left;
    if ( m_left ) {
        m_left->setParent( this );
    }
}
void CSGNode::setRight( SharedPointer< Node > const &right ) noexcept
{
    if ( m_right ) {
        m_right->setParent( nullptr );
    }
    m_right = right;
    if ( m_right ) {
        m_right->setParent( this );
    }
}

void CSGNode::accept( NodeVisitor &visitor )
{
    visitor.visitCSGNode( this );
}

void CSGNode::encode( coding::Encoder &encoder )
{
    Node::encode( encoder );
}

void CSGNode::decode( coding::Decoder &decoder )
{
    Node::decode( decoder );
}
