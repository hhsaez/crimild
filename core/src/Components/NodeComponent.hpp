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

#ifndef CRIMILD_COMPONENTS_NODE_COMPONENT_
#define CRIMILD_COMPONENTS_NODE_COMPONENT_

#include "Foundation/SharedObject.hpp"
#include "Mathematics/Time.hpp"

namespace crimild {

	class Node;
	class Renderer;
	class Camera;

	class NodeComponent : public SharedObject {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( NodeComponent )

	protected:
		NodeComponent( void );

	public:
		virtual ~NodeComponent( void );

		void setNode( Node *node ) { _node = node; }
		Node *getNode( void ) { return _node; }
		const Node *getNode( void ) const { return _node; }

		template< class NODE_TYPE >
		NODE_TYPE *getNode( void ) { return static_cast< NODE_TYPE * >( _node ); }

		virtual const char *getComponentName( void ) const { return "update"; }

	private:
		Node *_node;

	public:
		/**
		   \brief Invoked once when component is attached to a node
		*/
		virtual void onAttach( void );

		/**
		   \brief Invoked once when scene is loaded
		*/
		virtual void start( void );

		/**
		   \brief Invoked multiple times (usually once per simulation step)
		*/
		virtual void update( const Time &t );

		/**
			\brief Invoked only if debug rendering is enabled
		*/
		virtual void renderDebugInfo( Renderer *renderer, Camera *camera );

		/**
		   \brief Invoked once when component is detached from a node
		*/
		virtual void onDetach( void );

	};

}

// useful macro for declaring a component name
// _COMPONENT_NAME is for internal use only (see Node and NodeComponentCatalog classes)
#define CRIMILD_NODE_COMPONENT_NAME( X ) \
 	public: \
 		static const char *_COMPONENT_NAME( void ) { return X; } \
 		virtual const char *getComponentName( void ) const override { return _COMPONENT_NAME(); }

#endif

