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

#ifndef CRIMILD_SCENE_GRAPH_GROUP_NODE_
#define CRIMILD_SCENE_GRAPH_GROUP_NODE_

#include "Node.hpp"

#include <list>
#include <functional>

namespace Crimild {

	class GroupNode : public Node {
	public:
		explicit GroupNode( std::string name = "" );
		virtual ~GroupNode( void );

		bool hasNodes( void ) const { return ( _nodes.size() > 0 ); }
		void attachNode( NodePtr node );
		void detachNode( NodePtr node );
		void detachAllNodes( void );
		void foreachNode( std::function< void( NodePtr & ) > callback );

	private:
		std::list< NodePtr > _nodes;

	public:
		virtual void accept( NodeVisitor &visitor ) override;

	};

	typedef std::shared_ptr< GroupNode > GroupNodePtr;

}

#endif

