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

#include "NodeVisitor.hpp"
#include "SceneGraph/Node.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Light.hpp"
#include "SceneGraph/Text.hpp"

using namespace crimild;

NodeVisitor::NodeVisitor( void )
{

}

NodeVisitor::~NodeVisitor( void )
{

}

void NodeVisitor::reset( void )
{

}

void NodeVisitor::traverse( NodePtr const &node )
{
	reset();
	node->accept( *this );
}

void NodeVisitor::visitNode( NodePtr const &node )
{
	// do nothing
}

void NodeVisitor::visitGroup( GroupPtr const &group )
{
	// by default, just traverse to child nodes
	group->foreachNode( [&]( NodePtr const &node ) { node->accept( *this ); } );
}

void NodeVisitor::visitGeometry( GeometryPtr const &geometry )
{
	// by default, do the same as with any other node
	visitNode( geometry );
}

void NodeVisitor::visitText( TextPtr const &text )
{
    // by default, do the same as with geometries
    visitGeometry( text );
}

void NodeVisitor::visitCamera( CameraPtr const &camera )
{
	visitGroup( camera );
}

void NodeVisitor::visitLight( LightPtr const &light )
{
	visitNode( light );
}

