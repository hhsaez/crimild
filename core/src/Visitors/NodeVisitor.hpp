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

#ifndef CRIMILD_VISITORS_NODE_VISITOR_
#define CRIMILD_VISITORS_NODE_VISITOR_

#include "Foundation/Memory.hpp"

namespace crimild {

	class Node;
	class Group;
	class Geometry;
	class Camera;
	class Light;
    class Text;

	class NodeVisitor {
	protected:
		NodeVisitor( void );

	public:
		virtual ~NodeVisitor( void );

		virtual void reset( void );

        virtual void traverse( SharedPointer< Node > const &node );

		virtual void visitNode( SharedPointer< Node > const &node );
		virtual void visitGroup( SharedPointer< Group > const &group );
		virtual void visitGeometry( SharedPointer< Geometry > const &geometry );
        virtual void visitText( SharedPointer< Text > const &text );
		virtual void visitCamera( SharedPointer< Camera > const &camera );
		virtual void visitLight( SharedPointer< Light > const &light );

	private:
		NodeVisitor( const NodeVisitor & ) { }
		NodeVisitor &operator=( const NodeVisitor & ) { return *this; }
	};

}

#endif


