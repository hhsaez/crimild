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

namespace crimild {

	class Node;
	class Renderer;
	class Camera;

    class NodeComponent;
    
    using NodeComponentPtr = SharedPointer< NodeComponent >;

    class NodeComponent : public SharedObject {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( NodeComponent )
        
	protected:
		NodeComponent( void );

	public:
		virtual ~NodeComponent( void );

    public:
        // internal use only
        void setNode( Node *node ) { _node = node; }
		
        // internal use only
        Node *getNodePointer( void ) { return _node; }
        
    public:
        SharedPointer< Node > getNode( void );
        
        const SharedPointer< Node > getNode( void ) const;
        
		template< class NodeClass >
        SharedPointer< NodeClass > getNode( void ) { return crimild::castPointer< NodeClass >( getNode() ); }

		virtual const char *getComponentName( void ) const { return "update"; }

		bool isEnabled( void ) const { return _enabled; }
		void setEnabled( bool value ) { _enabled = value; }

	private:
        Node *_node;
        bool _enabled = true;
        
    public:
        NodeComponentPtr getComponentWithName( std::string name );
        
        template< class NODE_COMPONENT_CLASS >
        SharedPointer< NODE_COMPONENT_CLASS > getComponent( void )
        {
            return std::static_pointer_cast< NODE_COMPONENT_CLASS >( getComponentWithName( NODE_COMPONENT_CLASS::_COMPONENT_NAME() ) );
        }

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
			\brief Invoked only if debug rendering is enabled
		*/
        virtual void renderDebugInfo( SharedPointer< Renderer > const &renderer, SharedPointer< Camera > const &camera );

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

