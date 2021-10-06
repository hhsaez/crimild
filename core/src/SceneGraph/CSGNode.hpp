/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#ifndef CRIMILD_CORE_SCENEGRAPH_CSG_NODE_
#define CRIMILD_CORE_SCENEGRAPH_CSG_NODE_

#include "SceneGraph/Node.hpp"

namespace crimild {

    class CSGNode : public Node {
        CRIMILD_IMPLEMENT_RTTI( crimild::CSGNode )

    public:
        enum class Operator {
            UNION,
            INTERSECTION,
            DIFFERENCE,
        };

    public:
        CSGNode( void ) = default;
        explicit CSGNode( Operator op ) noexcept;
        CSGNode( Operator op, SharedPointer< Node > const &left, SharedPointer< Node > const &right ) noexcept;
        virtual ~CSGNode( void ) = default;

        inline Operator getOperator( void ) const noexcept { return m_operator; }

        inline Node *getLeft( void ) noexcept { return get_ptr( m_left ); }
        inline void setLeft( SharedPointer< Node > const &left ) noexcept { m_left = left; }

        inline Node *getRight( void ) noexcept { return get_ptr( m_right ); }
        inline void setRight( SharedPointer< Node > const &right ) noexcept { m_right = right; }

    private:
        Operator m_operator;
        SharedPointer< Node > m_left;
        SharedPointer< Node > m_right;

    public:
        virtual void accept( NodeVisitor &visitor ) override;

        /**
            \name Coding
         */
        //@{
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

}
#endif
