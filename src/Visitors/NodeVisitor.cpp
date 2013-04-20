/**
 * Crimild Engine is an open source scene graph based engine which purpose
 * is to fulfill the high-performance requirements of typical multi-platform
 * two and tridimensional multimedia projects, like games, simulations and
 * virtual reality.
 *
 * Copyright (C) 2006-2013 Hernan Saez - hhsaez@gmail.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "NodeVisitor.hpp"
#include "SceneGraph/Node.hpp"
#include "SceneGraph/GroupNode.hpp"

using namespace Crimild;

NodeVisitor::NodeVisitor( void )
{

}

NodeVisitor::~NodeVisitor( void )
{

}

void NodeVisitor::reset( void )
{

}

void NodeVisitor::traverse( Node *node )
{
	reset();
	node->accept( *this );
}

void NodeVisitor::visitNode( Node *node )
{
	// do nothing
}

void NodeVisitor::visitGroupNode( GroupNode *group )
{
	// by default, just traverse to child nodes
	group->foreachNode( [&]( NodePtr &node ) { node->accept( *this ); } );
}

