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
#include "Foundation/NamedObject.hpp"
#include "Foundation/Stream.hpp"
#include "Visitors/NodeVisitor.hpp"
#include "Components/NodeComponent.hpp"
#include "Mathematics/Transformation.hpp"
#include "Boundings/BoundingVolume.hpp"

#include <map>

namespace crimild {
    
	/**
		\brief Base class for any object that can be attached to the scene graph
	*/
	class Node : 
		public NamedObject, 
		public StreamObject {
        CRIMILD_IMPLEMENT_RTTI( crimild::Node )

	public:
		explicit Node( std::string name = "" );
		virtual ~Node( void );

	public:
		bool hasParent( void ) const { return _parent != nullptr; }

		Node *getParent( void ) { return _parent; }

        template< class NodeClass >
        NodeClass *getParent( void )
        {
            return static_cast< NodeClass * >( _parent );
        }
        
		void setParent( Node *parent ) { _parent = parent; }

		SharedPointer< Node > detachFromParent( void );

        Node *getRootParent( void );

        template< class NodeClass >
        NodeClass *getRootParent( void )
        {
            return static_cast< NodeClass * >( getRootParent() );
        }
        
	private:
		/**
			\brief A node's parent

			Every node if linked with its parent in the node hierarchy (provided
			one is available). 
		*/
        Node *_parent = nullptr;

	public:
		void perform( NodeVisitor &visitor );
		void perform( const NodeVisitor &visitor );

		virtual void accept( NodeVisitor &visitor );

	public:
        NodeComponent *getComponentWithName( std::string name )
        {
            return crimild::get_ptr( _components[ name ] );
        }
        
        template< class NODE_COMPONENT_CLASS >
        NODE_COMPONENT_CLASS *getComponent( void )
        {
            return static_cast< NODE_COMPONENT_CLASS * >( getComponentWithName( NODE_COMPONENT_CLASS::__CLASS_NAME ) );
        }
        
        bool hasComponent( SharedPointer< NodeComponent > const &component )
        {
            return hasComponent( crimild::get_ptr( component ) );
        }
        
        bool hasComponent( NodeComponent *component )
        {
            auto it = _components.find( component->getComponentName() );
            return ( it != _components.end() && crimild::get_ptr( it->second ) == component );
        }
        
		void attachComponent( NodeComponent *component );
        void attachComponent( SharedPointer< NodeComponent > const &component );
        
        template< typename T, typename... Args >
        T *attachComponent( Args &&... args )
        {
            auto cmp = crimild::alloc< T >( std::forward< Args >( args )... );
            attachComponent( cmp );
            return crimild::get_ptr( cmp );
        }
        
        void detachComponent( NodeComponent *component );
		void detachComponent( SharedPointer< NodeComponent > const &component );
		
        SharedPointer< NodeComponent > detachComponentWithName( std::string name );
		
        void detachAllComponents( void );

		void startComponents( void );

		void updateComponents( const Clock &clock );
		
		void forEachComponent( std::function< void ( NodeComponent * ) > callback );

	private:
		std::map< std::string, SharedPointer< NodeComponent >> _components;

	public:
		void setLocal( const Transformation &t ) { _local = t; }
		const Transformation &getLocal( void ) const { return _local; }
		Transformation &local( void ) { return _local; }

		void setWorld( const Transformation &t ) { _world = t; }
		const Transformation &getWorld( void ) const { return _world; }
		Transformation &world( void ) { return _world; }

		bool worldIsCurrent( void ) const { return _worldIsCurrent; }
		void setWorldIsCurrent( bool isCurrent ) { _worldIsCurrent = isCurrent; }

	private:
		Transformation _local;
		Transformation _world;

		/**
			\brief Indicates if the world transformation needs to be updated automatically

			This flag can be used to avoid the automatic update of world transformations. 
			By default, the engine will compute the world transformation for a node as 
			a function of its parent's one. If this flag is set to 'true' (default is 'false'),
			you need to provide a valid world matrix manually
		*/
		bool _worldIsCurrent;

	public:
        BoundingVolume *localBound( void ) { return crimild::get_ptr( _localBound ); }
		const BoundingVolume *getLocalBound( void ) const { return crimild::get_ptr( _localBound ); }
        void setLocalBound( BoundingVolume *bound ) { _localBound = std::move( crimild::retain( bound ) ); }

		BoundingVolume *worldBound( void ) { return crimild::get_ptr( _worldBound ); }
		const BoundingVolume *getWorldBound( void ) const { return crimild::get_ptr( _worldBound ); }
        void setWorldBound( BoundingVolume *bound ) { _worldBound = std::move( crimild::retain( bound ) ); }

	private:
		SharedPointer< BoundingVolume > _localBound;
		SharedPointer< BoundingVolume > _worldBound;

	public:
		void setEnabled( bool enabled ) { _enabled = enabled; }
		bool isEnabled( void ) { return _enabled; }

	private:
		bool _enabled = true;

		/**
			\name Streaming support
		*/
		//@{

	public:
		bool registerInStream( Stream &s ) override;
		void save( Stream &s ) override;
		void load( Stream &s ) override;

		//@}
	};

}

#endif

