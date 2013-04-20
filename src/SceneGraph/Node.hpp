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

#ifndef CRIMILD_SCENE_GRAPH_NODE_
#define CRIMILD_SCENE_GRAPH_NODE_

#include "Foundation/NamedObject.hpp"
#include "Visitors/NodeVisitor.hpp"
#include "Components/NodeComponent.hpp"

#include <map>

namespace Crimild {

	/**
		\brief Base class for any object that can be attached to the scene graph

	*/
	class Node : public NamedObject {
	public:
		explicit Node( std::string name = "" );
		virtual ~Node( void );

	public:
		bool hasParent( void ) const { return _parent != nullptr; }
		Node *getParent( void ) { return _parent; }
		void setParent( Node *parent ) { _parent = parent; }

	private:
		/**
			\brief A node's parent

			Every node if linked with its parent in the node hierarchy (provided
			one is available). 

			The reason for using a regular C++ pointer (instead of a smart pointer
			like weak_ptr) lies in the fact that a child node cannot claim 
			ownership over its parent node. A regular pointer emphasizes this. 
		*/
		Node *_parent;

	public:
		void perform( NodeVisitor &visitor );
		void perform( const NodeVisitor &visitor );

		virtual void accept( NodeVisitor &visitor );

	public:
		NodeComponent *getComponentWithName( std::string name );
		void attachComponent( NodeComponentPtr component );
		void detachComponent( NodeComponentPtr component );
		void detachComponentWithName( std::string name );
		void detachAllComponents( void );

		template< class T >
		T *getComponent( void )
		{
			return static_cast< T * >( _components[ T::NAME ].get() );
		}

		void updateComponents( void );

	private:
		std::map< std::string, NodeComponentPtr > _components;
	};

	typedef std::shared_ptr< Node > NodePtr;

}

#endif

