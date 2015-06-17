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

#ifndef CRIMILD_SCENE_GRAPH_NODE_
#define CRIMILD_SCENE_GRAPH_NODE_

#include "Foundation/SharedObject.hpp"
#include "Foundation/Pointer.hpp"
#include "Foundation/NamedObject.hpp"
#include "Visitors/NodeVisitor.hpp"
#include "Components/NodeComponent.hpp"
#include "Mathematics/Transformation.hpp"
#include "Boundings/BoundingVolume.hpp"

#include <map>

namespace crimild {
    
    class Node;
    
    using NodePtr = SharedPointer< Node >;
    using NodeWPtr = WeakPointer< Node >;

	/**
		\brief Base class for any object that can be attached to the scene graph

	*/
	class Node : public NamedObject, public SharedObject {
        CRIMILD_DISALLOW_COPY_AND_ASSIGN( Node )

	public:
		explicit Node( std::string name = "" );
		virtual ~Node( void );

	public:
		bool hasParent( void ) const { return pointerIsValid( _parent ); }

		NodePtr getParent( void ) { return getSharedPointer( _parent ); }

		void setParent( NodePtr const &parent ) { _parent = parent; }

		NodePtr detachFromParent( void );

		NodePtr getRootParent( void );

		template< class NodeClass >
        SharedPointer< NodeClass > getParent( void ) { return std::static_pointer_cast< NodeClass >( getSharedPointer( _parent ) ); }

	private:
		/**
			\brief A node's parent

			Every node if linked with its parent in the node hierarchy (provided
			one is available). 
		*/
        NodeWPtr _parent;

	public:
		void perform( NodeVisitor &visitor );
		void perform( const NodeVisitor &visitor );

		virtual void accept( NodeVisitor &visitor );

	public:
		NodeComponentPtr getComponentWithName( std::string name );
		void attachComponent( NodeComponentPtr const &component );
		void detachComponent( NodeComponentPtr const &component );
		void detachComponentWithName( std::string name );
		void detachAllComponents( void );

		template< class NODE_COMPONENT_CLASS >
        SharedPointer< NODE_COMPONENT_CLASS > getComponent( void )
		{
            return std::static_pointer_cast< NODE_COMPONENT_CLASS >( _components[ NODE_COMPONENT_CLASS::_COMPONENT_NAME() ] );
		}

		void startComponents( void );
		
		void foreachComponent( std::function< void ( NodeComponentPtr const & ) > callback );

	private:
		std::map< std::string, NodeComponentPtr > _components;

	public:
		void setLocal( const TransformationImpl &t ) { _local = t; }
		const TransformationImpl &getLocal( void ) const { return _local; }
		TransformationImpl &local( void ) { return _local; }

		void setWorld( const TransformationImpl &t ) { _world = t; }
		const TransformationImpl &getWorld( void ) const { return _world; }
		TransformationImpl &world( void ) { return _world; }

		bool worldIsCurrent( void ) const { return _worldIsCurrent; }
		void setWorldIsCurrent( bool isCurrent ) { _worldIsCurrent = isCurrent; }

	private:
		TransformationImpl _local;
		TransformationImpl _world;

		/**
			\brief Indicates if the world transformation needs to be updated automatically

			This flag can be used to avoid the automatic update of world transformations. 
			By default, the engine will compute the world transformation for a node as 
			a function of its parent's one. If this flag is set to 'true' (default is 'false'),
			you need to provide a valid world matrix manually
		*/
		bool _worldIsCurrent;

	public:
		BoundingVolumePtr localBound( void ) { return _localBound; }
		const BoundingVolumePtr getLocalBound( void ) const { return _localBound; }
		void setLocalBound( BoundingVolumePtr const &bound ) { _localBound = bound; }

		BoundingVolumePtr worldBound( void ) { return _worldBound; }
		const BoundingVolumePtr getWorldBound( void ) const { return _worldBound; }
		void setWorldBound( BoundingVolumePtr const &bound ) { _worldBound = bound; }

	private:
		BoundingVolumePtr _localBound;
		BoundingVolumePtr _worldBound;

	public:
		void setEnabled( bool enabled ) { _enabled = enabled; }
		bool isEnabled( void ) { return _enabled; }

	private:
		bool _enabled = true;
	};

}

#endif

